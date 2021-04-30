#include "mpx_supt.h"
#include "structs.h"
#include <string.h>
#include <mem/heap.h>
#include <core/serial.h>
#include <core/io.h>
#include "io_int_handlers.h"
#include <core/tables.h>

#define PIC_MASK 0x21
#define PIC_COMM 0x20

extern void first_level_int_isr();

struct dcb device = {.open_flag = 1};
u32int old_handler;
int *event_flag_copy;
u32int old_mask;

/**
*	@brief Initializes a new device control block
*		   to encapsulate the COM1 device, calculates
*		   baud rate divisor and enables interrupts
*	@param *eflag_p: pointer to event flag
*   @param baud_rate: baud rate for the serial port connection
*	@retval 0 if normal behavior
*	@retval -101 if invalid event flag
*	@retval -102 if invalid baud rate
*	@retval -103 if device is already open
*/
int com_open (int *eflag_p, int baud_rate){
	//cli();
	//Steps from R6 Detailed
	//1. Esnure that params are valid
	if(eflag_p == NULL)
		return -101;
	if(baud_rate <= 0)
		return -102;
	/*if (device != NULL)
		return -103;*/

	//2. Initialize the DCB

	//device = sys_alloc_mem(sizeof(struct dcb));
	device.open_flag = 1;
	device.event_flag = eflag_p;
	device.input_address = COM1;
	device.output_address = COM1;
	device.read_count = 0;
	device.write_count = 0;
	device.read_num_chars = 0;
	device.write_num_chars = 0;
	memset(device.ring_buffer, '\0', 100);
	device.ring_buf_pos = 0;
	event_flag_copy = eflag_p;
	device.status_code = 0;
	//3. Save address of the current interrupt handler
	//   Install new handler int interrupt vector
	old_handler = idt_get_gate(0x24);
	idt_set_gate(0x24, (u32int) first_level_int_isr, 0x08, 0x8e);
	//outb(0x0030, (u32int) &first_level_int_isr);
	//4. Compute the baud rate divisor
	/*int baud_rate_div = 115200 / (long) baud_rate;
	int brd_MSB = baud_rate_div & 0xFFFF;
	int brd_LSB = baud_rate_div & 0x0000;*/

	outb(COM1+1, 0x00); //disable interrupts
	//5. Store 0x80 in the Line Control Register
	outb(COM1+3, 0x80); //Write to Line Control

	//6. Store high and low order bits of the baud rate divisor
	outb(COM1 + 0, 115200/1200); //set bsd least sig bit
	outb(COM1 + 1, 0x00); //brd most significant bit

	//7. Store value 0x03 in Line Control Register
	outb(COM1+3, 0x03); //Write to Line Control

	//8. Enable PIC mask level 4
	old_mask = inb(PIC_MASK);
	int new_mask = old_mask & ~0x10;
	outb(PIC_MASK, new_mask);
	outb(COM1+4, 0x0B);//enable interrupts

	//9. Enable serial port interrupts
	//outb(COM1+4, 0x08); //Write to Modem Control
	//10. Enable input ready interrupts
	outb(COM1+1, 0x01); //Write to Interrupt Enable
	(void)inb(COM1);
	//sti();
	return 0;
}

/**
*	@brief Terminates the serial port connection to COM1
*	@param none
*	@retval 0 if normal behavior
*	@retval -201 if device is already closed
*/
int com_close (void){
	//1. Ensure port is open
	if(device.open_flag == 0)
		return -201;

	//2. Clear open indicator in DCB
	device.open_flag = 0;

	//3. Disable PIC mask level 4
	outb(COM1+1, 0x00); //disable interrupts
	outb(PIC_MASK, old_mask);
	outb(COM1+4, 0x0B);//enable interrupts

	//4. Disable all interrupts in the ACC
	//   by loading zero values to the Modem Status Register
	//   and the Interrupt Enable register
	outb(COM1+6, 0x00);
	outb(COM1+1, 0x00);
	//5. Restore original saved interrupt vector
	idt_set_gate(0x24, old_handler, 0x08, 0x8e);


	return 0;
}

/**
*	@brief Obtains characters from COM1 and loads them
*		   into the requestor's buffer.
*	@param *buf_p: pointer to requestor's buffer
*   @param count_p: address if int number of characters to be read
*	@retval 0 if normal behavior
*	@retval -301 serial port already open
*	@retval -302 if invalid character count
*	@retval -303 if device is already open
*   @retval -304 device is busy
*/
int com_read (char *buf_p, int *count_p){
	if(device.open_flag == 0)
		return -301;
	if(buf_p == NULL)
		return -302;
	if(count_p == NULL || *count_p < 0)
		return -303;
	if(device.status_code != 0)
		return -304;

	device.input = buf_p;
	device.read_count = *count_p;
	device.status_code = 1;
	device.event_flag = 0;

	int real_read_count = 0;
	//outb(COM1+1, 0x00); //disable interrupts
	cli();
	char letter = (device.ring_buffer)[0];
	while(real_read_count < device.read_count && letter != 13 && letter != NULL)
	{
		buf_p[real_read_count] = (device.ring_buffer)[real_read_count];
		(device.ring_buffer)[real_read_count] = NULL;
		real_read_count++;
		letter = (device.ring_buffer)[real_read_count];
	}
	sti();
	//outb(COM1+4, 0x0B); //enable interrupts

	device.status_code = 0;
	int set_flag = 1;
	int *set_flag_ptr = &set_flag;
	device.event_flag = set_flag_ptr;
	*count_p = real_read_count;
	return 0;
}

/**
*	@brief Initiates the transfer of a block of 
		   data to the serial port.
*	@param *buf_p: pointer to starting address of
			the buffer containing blocks to be written
*   @param count_p: pointer to int number of characters to be written
*	@retval 0 if normal behavior
*	@retval -401 serial port already open
*	@retval -402 if invalid character count
*	@retval -403 if device is already open
*   @retval -404 device is busy
*/
int com_write (char *buf_p, int *count_p){
	//cli();
	klogv("hello");
	if(device.open_flag == 0)
		return -401;
	if(buf_p == NULL)
		return -402;
	if(count_p == NULL || *count_p < 0)
		return -403;
	if(device.status_code != 0)
		return -404;

	device.output = buf_p;
	device.write_count = *count_p;
	device.status_code = 2;
	device.event_flag = 0;

	outb(COM1, buf_p[0]);
	u32int old_ier = inb(COM1+1);
	int new_ier = old_ier | 0x02;
	outb(COM1+1, (u32int) new_ier);

	//sti();
	return 0;
}
