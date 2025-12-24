/*
   Test src/mupen64plus.c
*/
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <dirent.h>

#include "jv_test.h"
#include "misc_test_utils.h"
#include "utils.h"
#include "mup.h"

#define ROM  "/Games/n64/roms/mk64.n64"



static void mup_test(void)
{
    int rv;
    char *rom_path = NULL;

    char *bin_path = "mupen64plus";
    char *sram_dir = "/save";
    char *resolution = "1920x1080";
    char *config_dir = "/config";

    mup_data_t md = {
        .bin = NULL,
        .sram_dir = NULL,
        .resolution = NULL,
        .config_dir = NULL
    };

    test_init();

    //
    // mup_init()
    //
    test_cond(mup_init(&md) == -1);
    md.bin = bin_path;
    test_cond(mup_init(&md) == -1);
    md.sram_dir = sram_dir;
    test_cond(mup_init(&md) == 0);
    md.resolution = "123x45a";
    test_cond(mup_init(&md) == -1);
    md.resolution = resolution;
    test_cond(mup_init(&md) == 0);
    md.config_dir = config_dir;
    test_cond(mup_init(&md) == 0);
    md.fullscreen = 3;
    test_cond(mup_init(&md) == 0);
    md.fullscreen = true;
    test_cond(mup_init(&md) == 0);
    md.wide_screen = false;
    test_cond(mup_init(&md) == 0);

    //
    // Bad ROM path
    //
    mup_load_rom("/fake/path/mk64.n64");
    test_cond(mup_start() == -1);

    //
    // Get ROM info
    //
    asprintf(&rom_path, "%s%s", getenv("HOME"), ROM);
    mup_load_rom(rom_path);
    free(rom_path);
    rv = mup_get_rom_info();
    test_cond(rv == 0);
    test_cond(mup_running() == false);
    test_cond(strlen(mup_rom_goodname()) != 0);
    test_cond(strlen(mup_rom_md5()) != 0);
    test_cond(strlen(mup_rom_country()) != 0);
    test_cond(strlen(mup_rom_type()) != 0);
    puts("---");
    printf("Goodname: \"%s\", \nMD5: \"%s\", \nCountry: \"%s\", \nImagetype: \"%s\"\n, ID: \"%s\"\n", 
            mup_rom_goodname(), mup_rom_md5(), mup_rom_country(), mup_rom_imagetype(), mup_rom_id());
    puts("---");

    //
    // Run
    //
    test_cond(mup_start() == 0);
    test_cond(mup_running() == true);

    mup_wait();

    //printf("Press any key to continue...");
    //getch_no_enter();
    //putchar('\n');
    //mup_stop();

    test_cond(mup_running() == false);

    test_results();
}

int main(void)
{
    putchar('\n');
    init_log_stream(NULL);

    mup_test();

    close_log_stream();

    return 0;
}
