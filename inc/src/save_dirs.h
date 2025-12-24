#ifndef SAVE_DIRS_H
#define SAVE_DIRS_H

#define CORE_SRAM_EQ  "Core[SaveSRAMPath]="  // --set option



/*
    Create all mupen64plus SRAM save directories
    --------
    - Returns 0 on success, -1 on failure
*/
int create_save_dirs(void);

/*
    Get save directory path
    ---------
    - categ: category string (path.h), ignored for CATEG_DEF type so can be NULL
    - type: CATEG_DEF, CATEG_REC, CATEG_USR
    - Returns path string, NULL on error
*/
#define CATEG_DEF  0  // Default, non-ghost play
#define CATEG_REC  1  // Record-holding ghosts
#define CATEG_USR  2  // User ghosts
    //
char* get_save_dir_path(const char *categ, int type);

/*
    Free resources
*/
void save_dirs_clean(void);



#endif
