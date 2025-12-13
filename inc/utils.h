/*
    Miscellaneous utilities
*/
#ifndef MK64TT_UTILS_H
#define MK64TT_UTILS_H

#include <stdio.h>
#include <string.h>



/*
    Concatenate strings
    -------
    - Combine maximum of <MAX_STRS> strings
    - Returns: 
        - String pointer
        - NULL on failure
    - Usage:
        char *str = concat_strings(str1, str2, str3);
        free(str);
*/
#define MAX_STRS  48
char *concat_strings_impl(int num_strings, ...);
    //
#define concat_strings(...) concat_strings_impl(MAX_STRS, __VA_ARGS__, NULL)



/*
    Create directory
    -------
    Functions like `mkdir -p`, creating parent directories as needed

    Parameters:
    - dir_path: absolute path
    - mode: directory permissions, e.g. 0755

    Returns:
    -  0: success
    - -1: failure
*/
int create_dir(const char *dir_path, int mode);



/*
    Logging
*/
extern FILE *log_file_stream;

int init_log_stream(const char *log_path);
int close_log_stream(void);

// Log formatted message
#define _log(...) \
do { \
    fprintf((log_file_stream), "%s: %s: %d: ", __FILE__, __func__, __LINE__); \
    fprintf((log_file_stream), __VA_ARGS__); \
    fprintf((log_file_stream), "\n"); \
} while (0)

// Log formatted message + errno
#define _loge(...) \
do { \
    _log(__VA_ARGS__); \
    fprintf((log_file_stream), "    errno: %s\n", strerror(errno)); \
} while (0)



#endif
