#include "mpx_supt.h"
#include "structs.h"
#include <string.h>
#include <mem/heap.h>
#include <core/serial.h>
#include <core/io.h>
#include "io_int_handlers.h"

/**
*	@brief This is level one of the 2-level
*	       serial port interrupt handler,
*		   which determines the exact cause of the
*		   interrupt and performing some general
*		   processing
*	@param DCB: pointer to the device control block for COM1
*	@retval void
*/
void first_level_int(struct dcb *DCB){
	klogv("hello interrupt");
	if (DCB->open_flag == 0){
		outb(0x20, 0x20);
		return;
	}
	if (inb(COM1 + 2) &  2){
		second_level_in(DCB);
	}
	else if (inb(COM1 + 2) & 4){
		second_level_out(DCB);
	}
	outb(0x20, 0x20);
}

/**
*	@brief This is level 2 of the 2-level
*	       serial port interrupt handler,
*		   which handles input interrupts
*	@param DCB: pointer to the device control block for COM1
*	@retval void
*/
int second_level_in(struct dcb *DCB){
	char letter = inb(COM1);
	if (DCB->status_code != 1){
		if (DCB->read_num_chars != 100){
			return -1;
		}
		else{
			DCB->ring_buffer[DCB->ring_buf_pos] = letter;
			return -1;
		}
	}
	else{
		DCB->input[DCB->read_num_chars] = letter;
		DCB->read_num_chars++;
		if (DCB->read_num_chars != 100 || letter != 13){
			return -1;
		}
		else{
			DCB->status_code = 0;
			int flag = 1;
			DCB->event_flag = &flag;
			return DCB->read_count;
		}
	}
}

/**
*	@brief This is level 2 of the 2-level
*	       serial port interrupt handler,
*		   which handles output interrupts
*	@param DCB: pointer to the device control block for COM1
*	@retval void
*/
int second_level_out(struct dcb *DCB){
	if (DCB->status_code != 2){
		return -1;
	}
	else if (DCB->write_num_chars != DCB->write_count){
		outb(COM1, DCB->output[DCB->write_num_chars]);
		DCB->write_num_chars++;
		return -1;
	}
	else{
		DCB->status_code = 0;
		int flag = 1;
		DCB->event_flag = &flag;
		outb(COM1 + 1, 0);
		return DCB->write_count;
	}
}