/*
   mupen64plus subprocess
*/
#ifndef MK64TT_MUPEN64PLUS_H
#define MK64TT_MUPEN64PLUS_H

#include <stdbool.h>



/*
    Initialize
    -------
    - Requires absolute ROM path
    - Returns 0 on sucess, -1 on failure
*/
int mupen64plus_init(const char *rom_path);

/*
    Start mupen64plus process
    ---------
    - Returns 0 on sucess, -1 on failure
*/
int mupen64plus_start(void);

/*
    Wait for mupen64plus to exit or be terminated
*/
int mupen64plus_wait(void);

/*
    Stop mupen64plus process
    ---------
    - Returns 0 on sucess, -1 on failure
*/
int mupen64plus_stop(void);

/*
    Get mupen64plus error message
    -------
    Usage: if (mup_err()) err = get_mupen64plus_err_msg();
*/
bool mup_err(void);
const char* get_mupen64plus_err_msg(void);

/*
    Get ROM info
*/
const char* get_rom_goodname(void);
const char* get_rom_country(void);
const char* get_rom_type(void);



#endif
