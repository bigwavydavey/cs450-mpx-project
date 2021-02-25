#ifndef _CMD_HANDLER_H
#define _CMD_HANDLER_H
#include <string.h>
#include <core/serial.h>
#include <core/io.h>

void settime(char* time_buffer, int time_buffer_size);

void gettime();

void setdate(char* date_buffer, int date_buffer_size);

void getdate();

void optional_cmd_handler(char * cmd_buffer);

void help();

void cmd_handler();

/**
  Description..: Allocates a block of memory (similar to malloc)
  @param Number of bytes to allocate
*/
#endif
