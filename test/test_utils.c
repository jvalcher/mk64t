/*
    Test src/utils/
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>
#include <errno.h>

#include "utils.h"
#include "_test_utils.h"



static void concat_strings_test(void)
{
    TEST_MSG;

    char *str1 = "Hello,";
    char *str2 = " world";
    char *str3 = "!";

    char *str = concat_strings(str1, str2, str3);
    assert(strcmp(str, "Hello, world!") == 0);

    free(str);

    PASSED;
}

static void create_dir_test(void)
{
    int rv;
    int mode = 0700;

    TEST_MSG;

    char *dir1 = "test_dir";
    rv = create_dir(dir1, mode);
    assert(rv != -1);
    assert(opendir(dir1) != NULL);
    remove(dir1);
    assert(opendir(dir1) == NULL);

    char *dir2 = "test_dir1/test_dir2";
    create_dir(dir2, mode);
    char *base_dir2 = strdup(dir2);
    base_dir2 = dirname(base_dir2);
    assert(opendir(dir2) != NULL);
    remove(dir2);
    remove(base_dir2);
    assert(opendir(dir2) == NULL);
    assert(opendir(base_dir2) == NULL);
    free(base_dir2);
    base_dir2 = NULL;

    PASSED;
}

static void log_test(void)
{
    char buf[1024];

    TEST_MSG;

    char *log_dir = concat_strings(getenv("HOME"), "/.local/share/test");
    char *log_path = concat_strings(log_dir, "log");

    // Create log file
    remove(log_path);
    remove(log_dir);
    create_dir(log_dir, 0700);
    init_log_stream(log_path);

    _log("Error opening file");
    _log("Unable to open path \"%s\"", "/my/path");
    errno = 1;
    _loge("System call failed");

    close_log_stream();

    // Check content
    FILE *f = fopen(log_path, "r");
    assert(f != NULL);
    fgets(buf, sizeof buf, f);
    assert(strstr(buf, "Error opening file") != NULL);
    fgets(buf, sizeof buf, f);
    assert(strstr(buf, "/my/path") != NULL);
    fgets(buf, sizeof buf, f);
    assert(strstr(buf, "System call failed") != NULL);
    fgets(buf, sizeof buf, f);
    assert(strstr(buf, "errno: Operation not permitted") != NULL);
    fclose(f);

    remove(log_path);
    remove(log_dir);
    assert(fopen(log_path, "r") == NULL);
    assert(opendir(log_dir) == NULL);
    free(log_dir);
    free(log_path);

    PASSED;
}

int main(void)
{
    concat_strings_test();
    create_dir_test();
    log_test();

    return 0;
}

