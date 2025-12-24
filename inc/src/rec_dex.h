/*
    Record ghost mpk data
*/
#ifndef GHOST_DATA_H
#define GHOST_DATA_H

#include <stddef.h>
#include <stdint.h>

#define DEX_HEADER_SIZE  4160



/*
    Dex blob size
*/
unsigned int rec_dex_data_size(void);

/*
    Get mpk data
    ---------
    - index: category index corresponding to mpk_categ_name[]
    - Returns mpk blob or NULL if categ_name not found
*/
const uint8_t* rec_dex_get_data(int index);



#endif
