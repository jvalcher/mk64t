#include <stdint.h>

#include "rec_mpk.h"
#include "categ.h"
#include "mpk.h"

#define MPK_SIZE  131072

static uint_8 curr_mpk[MPK_SIZE];



static int get_dex_index(const char *name)
{
    for (int i = 0; i < categ_cnt(); ++i)
        if (strcmp(name, categ_name[i]) == 0)
            return i;
    _logp("Unable to find dex index for \"%s\"", name);
    return -1;
}


static int dex_to_mpk(const uint8_t *dex)
{
    dex += DEX_HEADER_SIZE;
    memset(curr_mpk, 0, MPK_SIZE);
    for (size_t i = 0; i < MPK_SIZE; ++i)
        curr_mpk[i] = *dex++;

    return 0;
}

int get_rec_ghost(const char *categ_name, uint8_t **out_mpk)
{
    int idx;
    uint8_t *dex_data;

    if (rec_dex_data_size() <= DEX_HEADER_SIZE) {
        _logp("File too small to be DexDrive");
        return -1;
    }

    idx = get_dex_index(categ_name);
    if (idx == -1)
        return -1;

    dex_data = rec_dex_get_data(idx);
    if (!dex_data)
        return -1;

    if (dex_to_mpk(dex_data))
        return -1;

    *out_mpk = curr_mpk;

    return 0;
}

