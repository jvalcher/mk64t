#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <paths.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "utils.h"

#define LOG_DIR_MODE   0700
#define LOG_FILE_MODE  "w"

char *log_file_path = NULL;
FILE *log_file_stream = NULL;


int close_log_stream(void)
{
    errno = 0;
    if (fclose(log_file_stream) == EOF) {
        fprintf(stderr, "Failed to close log file: %s\n", strerror(errno));
        return -1;
    }
    log_file_stream = NULL;

    free(log_file_path);
    log_file_path = NULL;

    return 0;
}

static int open_log_file(const char *log_path)
{
    if (log_path == NULL) {
        fprintf(stderr, "Log file path is NULL\n");
        close_log_stream();
        return -1;
    }

    errno = 0;
    log_file_stream = fopen(log_path, LOG_FILE_MODE);
    if (log_file_stream == NULL) {
        fprintf(stderr, "Failed to open log file \"%s\": %s\n", 
                log_path, strerror(errno));
        return -1;
    }

    return 0;
}

int init_log_stream(const char *log_path)
{
    if (log_path == NULL)
        log_file_path = concat_strs(getenv("HOME"), LOG_DEFAULT_PATH);
    else
        log_file_path = strdup(log_path);

    if (open_log_file(log_file_path) == -1) {
        free(log_file_path);
        return -1;
    }

    return 0;
}

void print_see_log_msg(void)
{
    fprintf(stderr, "See log file for more information: %s\n", log_file_path);
}

