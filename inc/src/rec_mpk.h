#ifndef REC_MPK_H
#define REC_MPK_H


/*
    Get record-holding ghost for <categ_name>
    --------
    - categ_name: see categ.h
    - out_mpk: pointer for mpk buffer
    - Returns 0 on success, -1 on failure
*/
int get_rec_ghost(const char *categ_name, uint8_t **out_mpk);



#endif
