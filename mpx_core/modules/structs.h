#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <system.h>

/**
*	@brief This struct supports the 4 pcb queues used in MPX
*/
struct queue
{
  int count;
  struct pcb *head;
  struct pcb *tail;
};

/**
*	@brief This struct supports allocated and free queues of the heap manager
*/
struct cmcb_queue
{
	int count;
	struct cmcb *head;
	struct cmcb *tail;
};

struct io_queue
{
	int count;
	struct iocb *head;
	struct iocb *tail;
};

/**
*	@brief This struct encapsulates processes withing the MPX System
*/
struct pcb
{
	char name[10];
	int class;
	int priority;
	// 0- ready not suspended, 1- ready suspended, 2- blocked non suspended, 3- blcoked suspended, 5- running
	int state;
	unsigned char stack[2048];
	unsigned char *top;
	unsigned char *base;
	struct pcb *next;
	struct pcb *prev;
};

/**
*	@brief This struct stores a process's current state from the
*		   CPU registers to support context switches
*/
struct context
{
	u32int gs, fs, es, ds;
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32int eip, cs, eflags;
};

/**
*	@brief This struct supports the alarm process
*/
struct alarm
{
  char alarm_time[10];
  char alarm_msg[50];
  struct alarm *next;
  struct alarm *prev;
};

/**
*	@brief This struct stores user created alarms
*/
struct alarm_list
{
  int count;	
  struct alarm *head;
  struct alarm *tail;
};

/**
*	@brief This struct represents an allocated block of memory
*/
struct cmcb
{
	//0 Allocated, 1 Free
	int type;
	u32int beginning_address;
	int size;
	struct cmcb *next;
	struct cmcb *prev;
};

struct dcb
{
	int open_flag;
	int *event_flag;
	int status_code;
	char input[100];
	char output[100];
	u32int input_address;
	u32int output_address;
	char ring_buffer[100];
};

struct iocb
{
	struct pcb *process;
	struct dcb *device;
	int operation;
	char buffer[100];
	int buffer_size;
	struct iocb *next;
	struct iocb *prev;
};

struct queue ready_suspended;
struct queue ready_not_suspended;
struct queue blocked_suspended;
struct queue blocked_not_suspended;
u32int heap_address;

#endif
