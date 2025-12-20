/*
    src/mupen64plus.c
*/
#ifdef _WIN32
#else
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>

#ifdef _WIN32
#else
#include <spawn.h>
#include <signal.h>
#include <wait.h>
#include <pty.h>
#endif

#include "utils.h"
#include "ghost_data.h"
#include "mupen64plus.h"

#define STDOUT_FILENO  1
#define PIPE_READ  0
#define PIPE_WRITE  1

#define MUP_ERR_BUF_SIZE  256
#define ROM_INFO_BUF_SIZE  48

#ifdef _WIN32
#else
#define MUP_RAM_PATH  "/.local/share/mk64tt"
#endif

static int mup_pty_fd;
static pid_t mup_pid;

static int cmd_sram_opt_index = 2;
static int cmd_rom_index = 3;
static char *mup_cmd[] = {"mupen64plus", "--set", NULL, NULL, NULL };

static char mup_err_buf[MUP_ERR_BUF_SIZE];
static char rom_goodname[ROM_INFO_BUF_SIZE];
static char rom_country[ROM_INFO_BUF_SIZE];
static char rom_image_type[ROM_INFO_BUF_SIZE];
static bool mupen64plus_error = false;
static char *rom_info_not_found = "not found";

static int get_mup_info(void);



bool mup_err(void)
{
    return mupen64plus_error;
}

const char* get_mupen64plus_err_msg(void)
{
    return mup_err_buf;
}

const char* get_rom_goodname(void)
{
    return rom_goodname;
}

const char* get_rom_type(void)
{
    return rom_image_type;
}

const char* get_rom_country(void)
{
    return rom_country;
}

int mupen64plus_init(const char *rom_path)
{
    char *sram_opt = "Core[SaveSRAMPath]=";

    if (mup_cmd[cmd_rom_index] != NULL) {
        free(mup_cmd[cmd_rom_index]);
        mup_cmd[cmd_rom_index] = NULL;
    }
    if (mup_cmd[cmd_sram_opt_index] != NULL) {
        free(mup_cmd[cmd_sram_opt_index]);
        mup_cmd[cmd_sram_opt_index] = NULL;
    }

    // ROM path
    mup_cmd[cmd_rom_index] = strdup(rom_path);
    if (mup_cmd[cmd_rom_index] == NULL) {
        _logpe("ROM strdup failed");
        return -1;
    }

    // SRAM opt path
    mup_cmd[cmd_sram_opt_index] = concat_strs(sram_opt, getenv("HOME"), MUP_RAM_PATH, "/save");
    if (mup_cmd[cmd_sram_opt_index] == NULL) {
        _logp("Failed to create SRAM drectory path");
        return -1;
    }

    return 0;
}

int mupen64plus_start(void)
{
    _logp("Starting mupen64plus process...");

    mup_pid = forkpty(&mup_pty_fd, NULL, NULL, NULL);

    if (mup_pid == -1) {
        _logpe("mupen64plus fork failed");
        return -1;
    } 

    else if (mup_pid == 0) {
        execvp(mup_cmd[0], mup_cmd);
        perror("execvp mupen64plus");
        _exit(127);
    } 

    else {
        if (get_mup_info() == -1)
            return -1;
    }

    return 0;
}

int mupen64plus_wait(void)
{
    int wstatus;
    pid_t wpid;

    _logp("Waiting on mupen64plus process to exit...");

    do {
        wpid = waitpid(-mup_pid, &wstatus, 0);
    } while (wpid == -1 && errno == EINTR);
    if (wpid == -1) {
        _logpe("mupen64plus process waitpid failed");
        return -1;
    }

    if (WIFEXITED(wstatus)) {
        _logp("mupen64plus process exited (%d)", WEXITSTATUS(wstatus));
    } else if (WIFSIGNALED(wstatus)) {
        _logp("mupen64plus process exited by SIG%s%s", 
               sigabbrev_np(WTERMSIG(wstatus)), WCOREDUMP(wstatus) ? ", core dumped" : "");
    } else {
        _logp("mupen64plus process exited with unknown status (%d)", wstatus);
    }

    mup_pid = -1;
    close(mup_pty_fd);

    return 0;
}

int mupen64plus_stop(void)
{
    _logp("Stopping mupen64plus process...");

    if (mup_pid <= 0) {
        _logp("mupen64plus process already stopped");
        return 0;
    }

    if (kill(-mup_pid, SIGKILL) == -1) {
        if (errno == ESRCH)
            return 0;
        _logpe("mupen64plus process kill failed");   
        return -1;
    }

    mupen64plus_wait();

    return 0;
}

/*********
  Private
 *********/

/*
    Get info from mupen64plus terminal output
*/
static int get_mup_info(void)
{
    int i;
    ssize_t bytes = 0;
    int buf_offset;
    char buf[2048];
    char *ptr;

    char *ui_error = "UI-Console Error";
    char *goodname_key = "Goodname: ";
    char *image_type_key = "Imagetype: ";
    char *country_key = "Country: ";
    char *end_key = "UI-Console Status:";

    mupen64plus_error = false;

    mup_err_buf[0] = '\0';
    rom_goodname[0] = '\0';
    rom_image_type[0] = '\0';
    rom_country[0] = '\0';

    buf_offset = 0;
    for (;;) {
        if (buf_offset >= (int)(sizeof(buf) - 1))
            break;

        bytes = read(mup_pty_fd, buf + buf_offset, sizeof(buf)-buf_offset-1);
        if (bytes == -1 && errno != EAGAIN) {
            _logpe("Failed to read mupen64plus pipe");
            return -1;
        }
        if (bytes == 0)
            break;

        buf[buf_offset + bytes] = '\0';
        buf_offset += bytes;

        ptr = strstr(buf, ui_error);
        if (ptr != NULL)
            break;
        ptr = strstr(buf, end_key);
        if (ptr != NULL)
            break;
    }

    // UI-Console error
    ptr = strstr(buf, ui_error);
    if (ptr != NULL) {
        i = 0;
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < MUP_ERR_BUF_SIZE-1)
            mup_err_buf[i++] = *ptr++;
        mup_err_buf[i] = '\0';
        mupen64plus_error = true;
        _logp("mupen64plus %s", mup_err_buf);
        return -1;
    }

    // ROM Goodname
    ptr = strstr(buf, goodname_key);
    if (ptr != NULL) {
        ptr += strlen(goodname_key);
        i = 0;
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < ROM_INFO_BUF_SIZE-1)
            rom_goodname[i++] = *ptr++;
        rom_goodname[i] = '\0';
    } else {
        _logp("Failed to find ROM Goodname key in mupen64plus output");
        strcpy(rom_goodname, rom_info_not_found);
        return -1;
    }

    // ROM image type
    ptr = strstr(buf, image_type_key);
    if (ptr != NULL) {
        ptr += strlen(image_type_key);
        i = 0;
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < ROM_INFO_BUF_SIZE-1)
            rom_image_type[i++] = *ptr++;
        rom_image_type[i] = '\0';
    } else {
        _logp("Failed to find ROM Imagetype key in mupen64plus output");
        strcpy(rom_image_type, rom_info_not_found);
        return -1;
    }

    // ROM country
    ptr = strstr(buf, country_key);
    if (ptr != NULL) {
        ptr += strlen(country_key);
        i = 0;
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < ROM_INFO_BUF_SIZE-1)
            rom_country[i++] = *ptr++;
        rom_country[i] = '\0';
    } else {
        _logp("Failed to find ROM Country key in mupen64plus output");
        strcpy(rom_country, rom_info_not_found);
        return -1;
    }

    return 0;
}

