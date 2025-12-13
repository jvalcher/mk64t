#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>

#define STDOUT_FILENO  1
#define PIPE_READ  0
#define PIPE_WRITE  1

static pid_t mup_pid;
static int mup_out_pipe[2];
static char *mup_cmd[] = {"mupen64plus", NULL, NULL };
static int rom_index = 1;
static char country[24];
static char image_type[24];

static int get_info(void);



char* get_rom_country(void)
{
    return country;
}

char* get_rom_type(void)
{
    return image_type;
}


int mupen64plus_set_rom_path(const char *path)
{
    errno = 0;
    mup_cmd[rom_index] = strdup(path);
    if (mup_cmd[rom_index] == NULL) {
        perror("Failed to duplicate rom path");
        return -1;
    }

    return 0;
}

int mupen64plus_launch(void)
{
    if (pipe(mup_out_pipe) == -1) {
        perror("Failed to create mupen64plus out pipe");
        return -1;
    }

    mup_pid = fork();

    if (mup_pid == -1) {
        perror("mupen64plus fork failed");
        return -1;
    }

    else if (mup_pid == 0) {

        dup2(mup_out_pipe[PIPE_WRITE], STDOUT_FILENO);
        dup2(mup_out_pipe[PIPE_WRITE], STDERR_FILENO);
        close(mup_out_pipe[PIPE_WRITE]);
        close(mup_out_pipe[PIPE_READ]);

        errno = 0;
        if (execvp(mup_cmd[0], mup_cmd) == -1)
            perror("Failed to start mupen64plus process");
        exit(0);
    }

    else {
        close(mup_out_pipe[PIPE_WRITE]);

        if (get_info() == -1)
            return -1;
    }

    return 0;
}

int mupen64plus_stop(void)
{
    free(mup_cmd[rom_index]);

    errno = 0;
    if (kill(mup_pid, SIGTERM) == -1) {
        perror("mupen64plus sigterm failed");   
        return -1;
    }

    waitpid(mup_pid, NULL, 0);

    return 0;
}



/*********
  Private
 *********/

static int get_info(void)
{
    ssize_t bytes = 0;
    char buf[256];
    char *ptr;
    int i;

    char *rom_error_msg = "couldn\'t open ROM file";
    char *image_type_key = "Imagetype: ";
    char *country_key = "Country: ";
    bool image_type_found = false;
    bool country_found = false;

    while (!image_type_found && !country_found) {

        bytes = read(mup_out_pipe[PIPE_READ], buf, sizeof(buf)-1);
        if (bytes == -1) {
            perror("Failed to read mupen64plus pipe");
            return -1;
        }
        buf[bytes] = '\0';

        i = 0;

        // ROM error
        ptr = strstr(buf, rom_error_msg);
        if (ptr != NULL) {
            fprintf(stderr, "Invalid ROM");
            return -1;
        }

        // ROM image type
        ptr = strstr(buf, image_type_key);
        if (ptr != NULL) {
            ptr += strlen(image_type_key);
            while (*ptr != '\n' && *ptr != '\0')
                image_type[i++] = *ptr++;
            image_type[i] = '\0';
            image_type_found = true;
        }

        // ROM country
        ptr = strstr(buf, country_key);
        if (ptr != NULL) {
            ptr += strlen(country_key);
            while (*ptr != '\n' && *ptr != '\0')
                country[i++] = *ptr++;
            country[i] = '\0';
            country_found = true;
        }
    }

    return 0;
}

