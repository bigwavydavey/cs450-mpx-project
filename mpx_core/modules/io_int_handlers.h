#ifndef _IO_INT_HANDLERS
#define _IO_INT_HANDLERS

void first_level_int(struct dcb *DCB);
int second_level_in(struct dcb *DCB);
int second_level_out(struct dcb *DCB);

#endif