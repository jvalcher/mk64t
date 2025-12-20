/*
    Ghost mpk data
*/
#ifndef GHOST_DATA_H
#define GHOST_DATA_H

#include <stddef.h>

typedef struct {
    const char *name;
    const unsigned char *data;
} mpk_t;


/*
    Info
*/
size_t mpk_data_size(void);
size_t mpk_count(void);

/*
    Find mpk
    ---------
    - Return NULL if not found
*/
const mpk_t* get_mpk_by_index(size_t i);
const mpk_t* get_mpk_by_name(const char *name);



#endif
