/*
----------
Track, category strings
----------
"BB_3lap"
"BC_3lap"  
"CM_3lap"  
"CM_sc3lap"
"DK_3lap"  
"DK_sc3lap"
"FS_3lap"  
"KD_3lap"  
"KD_sc3lap"
"KTB_3lap" 
"LR_3lap"  
"LR_sc3lap"
"MMF_3lap" 
"MR_3lap"  
"MR_sc3lap"
"RRd_3lap" 
"RRd_sc3lap"
"RRy_3lap" 
"SL_3lap"  
"TT_3lap"  
"TT_sc3lap"
"WS_3lap"  
"WS_sc3lap"
"YV_3lap"
----------
*/
#ifndef TRACK_CATEG_H
#define TRACK_CATEG_H

#include <stddef.h>



/*
    Get track, category strings
    ----------
    - Usage:
        for (size_t i = 0; i < track_categ_cnt(); ++i)
            printf("%s\n", categ_name[i]);
*/
extern const char *track_categ_name[];
    //
size_t track_categ_cnt(void);



#endif
