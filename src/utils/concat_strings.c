#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "utils.h"


char* concat_strings_impl(int max_strs, ...)
{
    int str_len;
    int str_count;
    char *sub_str;
    char *str;
    va_list strs;
    
    // Calculate total string length
    str_len = 0;
    str_count = 0;
    va_start(strs, max_strs);
    for (str = va_arg(strs, char*);
         str != NULL;
         str = va_arg(strs, char*))
    {
        str_len += strlen(str);
        if (++str_count >= max_strs) {
            fprintf(stderr, "Max strings (%d) exceeded", max_strs);
            return NULL;
        }
    }
    va_end (strs);

    // Allocate string
    errno = 0;
    str = malloc(str_len + 1);
    if (str == NULL) {
        perror("string malloc failed");
        return NULL;
    }
    str [0] = '\0';

    // Create string
    va_start (strs, max_strs);
    for (sub_str = va_arg(strs, char*);
         sub_str != NULL;
         sub_str = va_arg(strs, char*))
    {
        strncat (str, sub_str, str_len - strlen(str));
    }
    va_end (strs);

    return str;
}

