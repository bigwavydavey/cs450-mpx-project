#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <system.h>

struct queue
{
  int count;
  struct pcb *head;
  struct pcb *tail;
};

struct mcb_queue
{
	int count;
	struct cmcb *head;
	struct cmcb *tail;
};

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

struct cmcb
{
	//0 Allocated, 1 Free
	int type;
	u32int beginning_address;
	int size;
	char pcb_name[10];
	struct cmcb *next;
	struct cmcb *prev;
};

struct lmcb
{
	int type;
	int size;
};

struct queue ready_suspended;
struct queue ready_not_suspended;
struct queue blocked_suspended;
struct queue blocked_not_suspended;

#endif
