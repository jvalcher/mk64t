/*
-----------------
Description: Single-header testing library
Author:      jvalcher
License:     MIT
-----------------
- Optional, formatted fail messages
- Colored output for easy review
- exit(1) on failure within test group/function

- Example usage:

    #include "jv_test.h"

    void run_tests_01(void) {
        test_init();

        test_cond     (x > 5, "%d is not greater than 5", x);
        test_strs_eq  (str1, str2, "\"%s\" not equal to \"%s\"", str1, str2);
        test_strs_neq (str2, str3);

        test_results();
    }

    int main(void) {
        run_tests_01();

        return 0;
    }
*/
#ifndef JV_TEST_H
#define JV_TEST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define R         "\033[1;0m"  // reset to default
#define RED       "\033[1;31m"
#define CYAN      "\033[1;36m"
#define GREEN     "\033[1;32m"
#define YELLOW    "\033[1;33m"
#define PURPLE    "\033[1;35m"
#define TEST_DIV      "---------------------"
#define TEST_OUT_DIV  "------Output---------"

int passed;
int failed;



/*
    Reset statistics, print function and file header
    --------
    - Run at start of every set of tests
*/
#define test_init() \
do { \
    passed = 0; \
    failed = 0; \
    printf (TEST_DIV CYAN "\n%s" R " :: " PURPLE "%s" R "()\n" TEST_OUT_DIV "\n", \
            __FILE__, __func__ \
    ); \
} while (0)

/*
    Print test results
    --------
    - Run at the end of every test function
*/
#define test_results() \
do { \
    printf (\
        TEST_DIV "\nPassed: " GREEN "%d" R ",  Failed: " RED "%d\n" R "\n", \
        passed, failed \
    ); \
    if (failed > 0) \
        exit(1); \
} while (0)

/*
    Print test failed message
    -------
    - Used by test functions below
    - Output:
        - Condition tested
        - Line number
        - Custom error message
    - Printed between test_init() and test_results() output
*/
#define print_test_failed(...) \
do { \
    printf (CYAN "  Line " YELLOW "%d" R "  ", __LINE__); \
    __VA_OPT__(printf (__VA_ARGS__)); \
    printf ("\n"); \
} while (0)

/****************
  Test functions
 ****************/
#define FAIL printf("    " RED "FAIL" R "  ")

/*
    Test conditional statement
*/
#define test_cond(condition, ...) \
do { \
    if (!(condition)) { \
        FAIL; \
        printf("(%s)", #condition); \
        print_test_failed(__VA_ARGS__); \
        ++failed; \
    } else { \
        ++passed; \
    } \
} while (0)

/*
    Test if strings equal
*/
#define test_strs_eq(str1, str2, ...) \
do { \
    if (strcmp((str1), (str2)) != 0) { \
        FAIL; \
        printf("(\"%s\" == \"%s\")", #str1, #str2); \
        print_test_failed(__VA_ARGS__); \
        ++failed; \
    } else { \
        ++passed; \
    } \
} while (0)

/*
    Test if strings not equal
*/
#define test_strs_neq(str1, str2, ...) \
do { \
    if (strcmp((str1), (str2)) == 0) { \
        FAIL; \
        printf("(\"%s\" != \"%s\")", #str1, #str2); \
        print_test_failed(__VA_ARGS__); \
    } else { \
        ++passed; \
    } \
} while (0)



#endif
