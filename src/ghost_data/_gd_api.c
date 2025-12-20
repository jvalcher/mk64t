#include <string.h>

#include "ghost_data.h"

extern unsigned char mpk_BB_3lap[];
extern unsigned char mpk_BC_3lap[];
extern unsigned char mpk_CM_3lap[];
extern unsigned char mpk_CM_sc3lap[];
extern unsigned char mpk_DK_3lap[];
extern unsigned char mpk_DK_sc3lap[];
extern unsigned char mpk_FS_3lap[];
extern unsigned char mpk_KD_3lap[];
extern unsigned char mpk_KD_sc3lap[];
extern unsigned char mpk_KTB_3lap[];
extern unsigned char mpk_LR_3lap[];
extern unsigned char mpk_LR_sc3lap[];
extern unsigned char mpk_MMF_3lap[];
extern unsigned char mpk_MR_3lap[];
extern unsigned char mpk_MR_sc3lap[];
extern unsigned char mpk_RRd_3lap[];
extern unsigned char mpk_RRd_sc3lap[];
extern unsigned char mpk_RRy_3lap[];
extern unsigned char mpk_SL_3lap[];
extern unsigned char mpk_TT_3lap[];
extern unsigned char mpk_TT_sc3lap[];
extern unsigned char mpk_WS_3lap[];
extern unsigned char mpk_WS_sc3lap[];
extern unsigned char mpk_YV_3lap[];

static mpk_t ghost_mpk_table[] = {
    { "BB_3lap",    mpk_BB_3lap },
    { "BC_3lap",    mpk_BC_3lap },
    { "CM_3lap",    mpk_CM_3lap },
    { "CM_sc3lap",  mpk_CM_sc3lap },
    { "DK_3lap",    mpk_DK_3lap },
    { "DK_sc3lap",  mpk_DK_sc3lap },
    { "FS_3lap",    mpk_FS_3lap },
    { "KD_3lap",    mpk_KD_3lap },
    { "KD_sc3lap",  mpk_KD_sc3lap },
    { "KTB_3lap",   mpk_KTB_3lap },
    { "LR_3lap",    mpk_LR_3lap },
    { "LR_sc3lap",  mpk_LR_sc3lap },
    { "MMF_3lap",   mpk_MMF_3lap },
    { "MR_3lap",    mpk_MR_3lap },
    { "MR_sc3lap",  mpk_MR_sc3lap },
    { "RRd_3lap",   mpk_RRd_3lap },
    { "RRd_sc3lap", mpk_RRd_sc3lap },
    { "RRy_3lap",   mpk_RRy_3lap },
    { "SL_3lap",    mpk_SL_3lap },
    { "TT_3lap",    mpk_TT_3lap },
    { "TT_sc3lap",  mpk_TT_sc3lap },
    { "WS_3lap",    mpk_WS_3lap },
    { "WS_sc3lap",  mpk_WS_sc3lap },
    { "YV_3lap",    mpk_YV_3lap }
};

extern unsigned int mpk_BB_3lap_len;



size_t mpk_data_size(void)
{
    return (size_t)mpk_BB_3lap_len;
}

size_t mpk_count(void)
{
    return (size_t)(sizeof(ghost_mpk_table) / sizeof(ghost_mpk_table[0]));
}

const mpk_t* get_mpk_by_index(size_t i)
{
    if (i < 0 || i >= mpk_count())
        return NULL;
    return &ghost_mpk_table[i];
}

const mpk_t* get_mpk_by_name(const char *name)
{
    for (size_t i = 0; i < mpk_count(); ++i)
        if (strcmp(name, ghost_mpk_table[i].name) == 0)
            return &ghost_mpk_table[i];
    return NULL;
}


