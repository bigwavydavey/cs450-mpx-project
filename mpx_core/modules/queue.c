#include "mpx_supt.h"
//#include (Will probably need file with PCB struct)

struct queue
{
  int count;
  pcb *head;
  pcb *tail;
}

struct pcb
{
	char[100] name;
	char[100] class;
	int priority;
	// 0- ready not suspended, 1- ready suspended, 2- blocked non suspended, 3- blcoked suspended, 5- running
	int state;
	unsigned char[1024] stack;
	unsigned char* top;
	unsigned char* base;
	pcb *next;
	pcb *previous;
}