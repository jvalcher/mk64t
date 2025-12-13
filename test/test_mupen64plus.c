/*
   Test src/mupen64plus.c
*/
#include <stdlib.h>
#include <assert.h>

#include "_test_utils.h"
#include "utils.h"
#include "mupen64plus.h"

static void mupen64plus_test(void)
{
    int rv;

    mupen64plus_set_rom_path("/fake/path/mk64.n64");
    rv = mupen64plus_launch();
    assert(rv == -1);
    mupen64plus_stop();

    char *rom_path = concat_strings(getenv("HOME"), "/Games/n64/roms/mk64.n64");
    mupen64plus_set_rom_path(rom_path);
    rv = mupen64plus_launch();
    assert(rv == 0);
    mupen64plus_stop();

    putchar('\n');
    free(rom_path);
}

int main(void)
{
    mupen64plus_test();

    return 0;
}
