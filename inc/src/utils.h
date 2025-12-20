/*
    Miscellaneous utilities
*/
#ifndef MK64TT_UTILS_H
#define MK64TT_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>



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
char *concat_strs_impl(int num_strings, ...);
    //
#define concat_strs(...) concat_strs_impl(MAX_STRS, __VA_ARGS__, NULL)



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
    Log, print formatted error messages
    ----------
    - Example usage:

        init_log_stream(NULL);
        if (LOG_STREAM_OPEN) {
            _log("error: %s", str);
            _loge("error: %s", str);
            _logp("error: %s", str);
            _logep("error: %s", str);
        }
        close_log_stream();
        print_see_log_msg();
*/
#ifdef _WIN32
#else
#define LOG_DEFAULT_PATH   "/.local/share/mk64tt/log"
#endif

#define LOG_STREAM_OPEN  log_file_stream

extern FILE *log_file_stream;

//
// Open log file stream
// -----------
// - log_path: absolute path to log file, NULL to use LOG_DEFAULT_PATH
// - Returns 0 on success, -1 on error
//
int init_log_stream(const char *log_path);

//
// Close log file stream
// --------
// - Returns 0 on success, -1 on error
//
int close_log_stream(void);

//
// Log
//
#define _log(...) \
do { \
    if (log_file_stream) { \
        fprintf((log_file_stream), "%s: %s: %d: ", __FILE__, __func__, __LINE__); \
        fprintf((log_file_stream), __VA_ARGS__); \
        fprintf((log_file_stream), "\n"); \
    } \
} while (0)

//
// Log errno
//
#define _loge(...) \
do { \
    _log(__VA_ARGS__); \
    if (log_file_stream) \
        fprintf((log_file_stream), "    errno: %s\n", strerror(errno)); \
} while (0)

//
// Log + print
//
#define _logp(...) \
do { \
    _log(__VA_ARGS__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
} while (0)

//
// Log + print errno
//
#define _logpe(...) \
do { \
    _loge(__VA_ARGS__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, ": %s\n", strerror(errno)); \
} while (0)

// Print log path information message
void print_see_log_msg(void);



#endif
