/*
   Test src/mupen64plus.c
*/
#include <stdlib.h>
#include <assert.h>

#include "jv_test.h"
#include "misc_test_utils.h"
#include "utils.h"
#include "mupen64plus.h"

#define ROM  "/Games/n64/roms/mk64.n64"

int passed, failed;

static void mupen64plus_test(void)
{
    int rv;
    char *rom_path = NULL;

    test_init();

    mupen64plus_init("/fake/path/mk64.n64");
    rv = mupen64plus_start();
    test_cond(rv == -1, "%d != -1", rv);

    rom_path = concat_strs(getenv("HOME"), ROM);
    mupen64plus_init(rom_path);
    rv = mupen64plus_start();
    test_cond(rv == 0, "%d != -1", rv);
    printf("Goodname: \"%s\", country: %s, type: %s\n", 
            get_rom_goodname(), get_rom_country(), get_rom_type());
    mupen64plus_wait();
    mupen64plus_stop();

    putchar('\n');
    free(rom_path);

    test_results();
}

int main(void)
{
    init_log_stream(NULL);

    mupen64plus_test();

    close_log_stream();

    return 0;
}
