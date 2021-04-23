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
#define COM1_VECTOR 0x30

extern void first_level_int_isr();

struct dcb *device;
u32int old_handler;
u32int old_mask;
int com_open (int *eflag_p, int baud_rate){
	//Steps from R6 Detailed
	//1. Esnure that params are valid
	if(eflag_p == NULL)
		return -101;
	if(baud_rate <= 0)
		return -102;
	if (device != NULL)
		return -103;
	
	//2. Initialize the DCB
	
	device = sys_alloc_mem(sizeof(struct dcb));
	device->open_flag = 1;
	device->event_flag = eflag_p;
	device->status_code = 0;
	device->input_address = COM1;
	device->output_address = COM1;
	device->read_count = 0;
	device->write_count = 0;
	device->read_num_chars = 0;
	device->write_num_chars = 0;
	strcpy(device->ring_buffer, " ");
	device->ring_buf_pos = 0;

	//3. Save address of the current interrupt handler
	//   Install new handler int interrupt vector
	old_handler = idt_get_gate(0x24);
	idt_set_gate(0x24, (u32int) first_level_int_isr, 0x08, 0x8e);

	//4. Compute the baud rate divisor
	int baud_rate_div = 115200 / (long) baud_rate;
	int brd_MSB = baud_rate_div & 0xFFFF0000;
	int brd_LSB = baud_rate_div & 0x0000FFFF;

	//5. Store 0x80 in the Line Control Register
	outb(COM1+3, 0x08); //Write to Line Control

	//6. Store high and low order bits of the baud rate divisor
	outb(COM1, (u32int) brd_LSB); //Write to Baud Rate LSB
	outb(COM1+1, (u32int) brd_MSB);//Write to Baude Rate MSB

	//7. Store value 0x03 in Line Control Register
	outb(COM1+3, 0x03); //Write to Line Control
	
	//8. Enable PIC mask level 4
	outb(COM1+1, 0x00); //disable interrupts
	old_mask = inb(PIC_MASK);
	int new_mask = old_mask & ~0x40;
	outb(PIC_MASK, new_mask);
	outb(COM1+4, 0x0B);//enable interrupts

	//9. Enable serial port interrupts
	outb(COM1+4, 0x08); //Write to Modem Control
	//10. Endable input ready interrupts
	outb(COM1+1, 0x01); //Write to Interrupt Enable

	return 0;
}

int com_close (void){
	//1. Ensure port is open
	if(device->open_flag == 0)
		return -201;

	//2. Clear open indicator in DCB
	device->open_flag = 0;

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

int com_read (char *buf_p, int *count_p){
	if(device->open_flag == 0)
		return -301;
	if(buf_p == NULL)
		return -302;
	if(count_p == NULL || *count_p < 0)
		return -303;
	if(device->status_code != 0)
		return -304;

	strcpy(device->input, " ");
	strcpy(device->output, " ");
	device->status_code = 1;
	int read_count = 0;

	outb(COM1+1, 0x00); //disable interrupts
	char letter = (device->ring_buffer)[0];
	while(read_count < *count_p && letter != '\r' && letter != NULL)
	{
		buf_p[read_count] = (device->ring_buffer)[read_count];
		(device->ring_buffer)[read_count] = NULL;
		read_count++;
	}
	outb(COM1+4, 0x0B); //enable interrupts
	device->status_code = 0;
	*count_p = read_count;
	return 0;
}

int com_write (char *buf_p, int *count_p){
	if(device->open_flag == 0)
		return -401;
	if(buf_p == NULL)
		return -402;
	if(count_p == NULL || *count_p < 0)
		return -403;
	if(device->status_code != 0)
		return -404;

	strcpy(device->output, " ");
	device->write_count = *count_p;
	device->status_code = 2;

	outb(COM1, buf_p[0]);
	u32int old_ier = inb(COM1+1);
	int new_ier = old_ier | 0x02;
	outb(COM1+1, (u32int) new_ier);

	return 0;
}