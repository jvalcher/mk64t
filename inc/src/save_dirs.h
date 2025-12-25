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
    - track_categ: track, category string (track_categ.h)
        - Ignored for GHOST_DEF type so can be NULL
    - ghost_type: GHOST_DEF, GHOST_REC, GHOST_USR
    - Returns path string, NULL on error
*/
#define GHOST_DEF  0  // Default, non-ghost play
#define GHOST_REC  1  // Record-holding ghosts
#define GHOST_USR  2  // User ghosts
    //
char* get_save_dir_path(const char *track_categ, int ghost_type);

/*
    Free resources (not directories)
*/
void save_dirs_clean(void);



#endif
