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
	sys_alloc_mem(sizeof(PCB));
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
	// checks ready_suspended
	struct pcb *current= ready_suspended.head;
	while (current != NULL)
	{
		if (strcmp(current->name, processName) == 0)
		{
			return current;
		}
		current = current->next;
	}

	// checks ready_not_suspended
	current= ready_not_suspended.head;
	while (current != NULL)
	{
		if (strcmp(current->name, processName) == 0)
		{
			return current;
		}
		current = current->next;
	}

	//checks blocked_suspended;
	current= blocked_suspended.head;
	while (current != NULL)
	{
		if (strcmp(current->name, processName) == 0)
		{
			return current;
		}
		current = current->next;
	}

	//checks blocked_not_suspended
	current= blocked_not_suspended.head;
	while (current != NULL)
	{
		if (strcmp(current->name, processName) == 0)
		{
			return current;
		}
		current = current->next;
	}
	
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
		sys_req(WRITE, DEFAULT_DEVICE, failure, failure_len);
	}
	else{
		sys_req(WRITE, DEFAULT_DEVICE, success, success_len);
	}
}

void InsertPCB(struct pcb *PCB){
/*
	if tail has higher priority than PCB
		put on tail of queue
	else
		search queue for priority, then insert at end of priority
*/
	if (PCB->state == 0){
		//ready_not_suspended queue
		if (ready_not_suspended.tail->priority >= PCB->priority)
		{
			ready_not_suspended.tail->next= PCB;
			PCB->prev = ready_not_suspended.tail;
			ready_not_suspended.tail = PCB;
		}
		else
		{
			struct pcb *current= ready_not_suspended.head;
			while(current->priority >= PCB->priority)
			{
				current = current->next;
			}
			current->next->prev = PCB;
			PCB->next = current->next;
			PCB->prev = current;
			current->next = PCB;
		}
		
	}
	else if (PCB->state == 1){
		//ready_suspended
		if (ready_suspended.tail->priority >= PCB->priority)
		{
			ready_suspended.tail->next= PCB;
			PCB->prev = ready_suspended.tail;
			ready_suspended.tail = PCB;
		}
		else
		{
			struct pcb *current= ready_suspended.head;
			while(current->priority >= PCB->priority)
			{
				current = current->next;
			}
			current->next->prev = PCB;
			PCB->next = current->next;
			PCB->prev = current;
			current->next = PCB;
		}
	}
	else if (PCB->state == 2){
		//blocked_non_suspended
		if (blocked_not_suspended.tail->priority >= PCB->priority)
		{
			blocked_not_suspended.tail->next= PCB;
			PCB->prev = blocked_not_suspended.tail;
			blocked_not_suspended.tail = PCB;
		}
		else
		{
			struct pcb *current= blocked_not_suspended.head;
			while(current->priority >= PCB->priority)
			{
				current = current->next;
			}
			current->next->prev = PCB;
			PCB->next = current->next;
			PCB->prev = current;
			current->next = PCB;
		}

	}
	else{
		//blocked_suspended
		if (blocked_suspended.tail->priority >= PCB->priority)
		{
			blocked_suspended.tail->next= PCB;
			PCB->prev = blocked_suspended.tail;
			blocked_suspended.tail = PCB;
		}
		else
		{
			struct pcb *current= blocked_suspended.head;
			while(current->priority >= PCB->priority)
			{
				current = current->next;
			}
			current->next->prev = PCB;
			PCB->next = current->next;
			PCB->prev = current;
			current->next = PCB;
		}
	}

}

void RemovePCB(struct pcb *PCB){
	//remove from queue
	//return success or error code
	char * success = "Success!\0";
	int success_size = strlen(success);
	int *success_len = &success_size;
	char * failure = "ERROR: The PCB could not be removed\0";
	int failure_size = strlen(failure);
	int *failure_len = &failure_size;

	struct pcb *toRemove = FindPCB(PCB->name);
	if (toRemove == NULL){
		sys_req(WRITE, DEFAULT_DEVICE, failure, failure_len);
	}

	toRemove->prev->next = toRemove->next;
	toRemove->next->prev = toRemove->prev;

	sys_req(WRITE, DEFAULT_DEVICE, success, success_len);
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