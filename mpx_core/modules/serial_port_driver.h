#ifndef SERIAL_PORT_DRIVER_H
#define SERIAL_PORT_DRIVER_H

extern struct dcb *device;

int com_open (int *eflag_p, int baud_rate);

int com_close (void);

int com_read (char *buf_p, int *count_p);

int com_write (char *buf_p, int *count_p);

#endif
