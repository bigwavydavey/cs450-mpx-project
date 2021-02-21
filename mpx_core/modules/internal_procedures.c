#include "mpx_supt.h"
#include "queue.c"
#include <string.h>

struct queue ready_suspended;
struct queue ready_not_suspended;
struct queue blocked_suspended;
struct queue blocked_not_suspended;

struct pcb * AllocatePCB(){
	int stack_size = 1024;
	struct pcb PCB;
	PCB.size = strlen(PCB.name) + stack_size;
	sys_alloc_mem(PCB.size);
	PCB.base = PCB.stack;
	PCB.top = PCB.base + stack_size;
	int loop_control = (int)PCB.base;
	int loop_end = (int) PCB.top;
	while (loop_control <= loop_end){
		PCB.stack[loop_control] = '\0';
		loop_control++;
	}
	struct pcb *pcb_point = &PCB;
	return pcb_point;
	return NULL;
}

struct pcb * FindPCB(char *processName){
	/*
		search queues for PCB with processName
		return that PCB pointer
	*/
	return NULL;
}

void FreePCB(struct pcb *PCB){
	char * success = "Success!\0";
	int success_size = strlen(success);
	int *success_len = &success_size;
	char * failure = "ERROR: The PCB could not be freed from memory\0";
	int failure_size = strlen(failure);
	int *failure_len = &failure_size;
	if (sys_free_mem(PCB) == -1){
		sys_req(WRITE, DEFAULT_DEVICE, failure, success_len);
	}
	else{
		sys_req(WRITE, DEFAULT_DEVICE, success, failure_len);
	}
}

void InsertPCB(struct pcb *PCB){
/*
	if ready
		search queue for priority, then insert at end of priority
	else
		put on tail of queue
*/
	if (PCB->state == 0){
		//ready_not_suspended queue
	}
	else if (PCB->state == 1){
		//ready_suspended
	}
	else if (PCB->state == 2){
		//blocked_non_suspended
		blocked_not_suspended.tail->next = PCB;
		PCB->prev = blocked_not_suspended.tail;
		blocked_not_suspended.tail = PCB;

	}
	else{
		//blocked_suspended
		blocked_suspended.tail->next = PCB;
		PCB->prev = blocked_suspended.tail;
		blocked_suspended.tail = PCB;
	}

}

void RemovePCB(struct pcb *PCB){
	FindPCB(PCB->name);
	//remove from queue
	//return success or error code
}

struct pcb * SetupPCB(char * processName, int class, int priority){
	struct pcb PCB;
	struct pcb *pcb_point = &PCB;
	pcb_point = AllocatePCB();
	strcpy(pcb_point->name, processName);
	pcb_point->class = class;
	pcb_point->priority = priority;
	pcb_point->state = 0;
	InsertPCB(pcb_point);
	return pcb_point;
}