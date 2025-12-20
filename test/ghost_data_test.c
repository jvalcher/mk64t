#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>

#include "_test_utils.h"
#include "utils.h"
#include "ghost_data.h"

#define MPK_DIR  "/../data/ghost/mpk"

int passed, failed;


void ghost_data_test(void)
{
    test_init();

    struct dirent *de = NULL;
    char *cwd = getcwd(NULL, 0);
    char *mpk_dir_path = concat_strings(cwd, MPK_DIR);
    DIR *mpk_dir = NULL;
    size_t mpk_file_count;

    //
    // mpk_dir/*.mpk count == mpk_count()
    //
    errno = 0;
    mpk_dir = opendir(mpk_dir_path);
    if (mpk_dir == NULL)
        perror("opendir mpk dir failed");
    assert(mpk_dir != NULL);

    mpk_file_count = 0;
    for (;;) {
        de = readdir(mpk_dir);
        if (de == NULL) 
            break;
        if (strcmp(de->d_name, ".") == 0 ||
            strcmp(de->d_name, "..") == 0)
            continue;
        ++mpk_file_count;
    }
    assert(mpk_file_count == mpk_count());

    closedir(mpk_dir);
    free(mpk_dir_path);
    free(cwd);

    test_results();
}

int main(void)
{
    init_log_stream();

    ghost_data_test();

    close_log_stream();

    return 0;
}
