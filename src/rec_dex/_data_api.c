#include <string.h>
#include <stdint.h>

#include "rec_dex.h"

extern uint8_t dex_BB_3lap[];
extern uint8_t dex_BC_3lap[];
extern uint8_t dex_CM_3lap[];
extern uint8_t dex_CM_sc3lap[];
extern uint8_t dex_DK_3lap[];
extern uint8_t dex_DK_sc3lap[];
extern uint8_t dex_FS_3lap[];
extern uint8_t dex_KD_3lap[];
extern uint8_t dex_KD_sc3lap[];
extern uint8_t dex_KTB_3lap[];
extern uint8_t dex_LR_3lap[];
extern uint8_t dex_LR_sc3lap[];
extern uint8_t dex_MMF_3lap[];
extern uint8_t dex_MR_3lap[];
extern uint8_t dex_MR_sc3lap[];
extern uint8_t dex_RRd_3lap[];
extern uint8_t dex_RRd_sc3lap[];
extern uint8_t dex_RRy_3lap[];
extern uint8_t dex_SL_3lap[];
extern uint8_t dex_TT_3lap[];
extern uint8_t dex_TT_sc3lap[];
extern uint8_t dex_WS_3lap[];
extern uint8_t dex_WS_sc3lap[];
extern uint8_t dex_YV_3lap[];

extern unsigned int dex_BB_3lap_size;

// Indexes match those in categ_name[]
static uint8_t *rec_dex_data[] = {
    dex_BB_3lap,
    dex_BC_3lap,
    dex_CM_3lap,
    dex_CM_sc3lap,
    dex_DK_3lap,
    dex_DK_sc3lap,
    dex_FS_3lap,
    dex_KD_3lap,
    dex_KD_sc3lap,
    dex_KTB_3lap,
    dex_LR_3lap,
    dex_LR_sc3lap,
    dex_MMF_3lap,
    dex_MR_3lap,
    dex_MR_sc3lap,
    dex_RRd_3lap,
    dex_RRd_sc3lap,
    dex_RRy_3lap,
    dex_SL_3lap,
    dex_TT_3lap,
    dex_TT_sc3lap,
    dex_WS_3lap,
    dex_WS_sc3lap,
    dex_YV_3lap
};



unsigned int rec_dex_data_size(void)
{
    return dex_BB_3lap_size;
}

const uint8_t* rec_dex_get_data(int index)
{
    if (index >= rec_dex_count()) {
        _logp("Dex index (%ld) out of range", index);
        return NULL;
    }
    return rec_dex_data[index];
}

