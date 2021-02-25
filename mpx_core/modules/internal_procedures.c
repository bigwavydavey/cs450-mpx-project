#include "mpx_supt.h"
#include <core/serial.h>
#include "structs.h"
#include <string.h>

struct queue ready_suspended;
struct queue ready_not_suspended;
struct queue blocked_suspended;
struct queue blocked_not_suspended;

/**
* This function is used to allocate memory for a pcb
* and initializes the stack to null
*
*  @retval pcb*: returns a pcb pointer 
* 
*/
struct pcb * AllocatePCB(){
	int stack_size = 1024;
	struct pcb *PCB;
	PCB = sys_alloc_mem(sizeof(PCB));
	PCB->base = PCB->stack;
	PCB->top = PCB->base + stack_size;
	int loop_control = 0;
	//int loop_end = (int) PCB->top;
	while (loop_control < stack_size){
		PCB->stack[loop_control] = '\0';
		loop_control++;
	}
	return PCB;
	return NULL;
}

/**
* This function is used to search through the 4 queues to find a specific pcb
*
* @param  processName: The name of the process is passed in as a pointer
* 
*
*  @retval pbc*: returns a pcb pointer
* 
*/
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

/**
* This function is used to free a pcb from memory  
* Success is printed if the command is successful
* if an the pcb is not freed Error is printed
*
* @param  PCB: the functions takes in a pcb pointer 
* 
*/
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

/**
* This function is used to insert a pcb into its correct queue
*
* @param  PCB: pcb pointer 
*
*/
void InsertPCB(struct pcb *PCB){
/*
	if tail has higher priority than PCB
		put on tail of queue
	else
		search queue for priority, then insert at end of priority
*/
	if (PCB->state == 0){
		//ready_not_suspended queue
		if (ready_not_suspended.count == 0){
			ready_not_suspended.head = PCB;
			ready_not_suspended.tail = PCB;
		}
		else if (ready_not_suspended.tail->priority >= PCB->priority)
		{
			ready_not_suspended.tail->next= PCB;
			PCB->prev = ready_not_suspended.tail;
			ready_not_suspended.tail = PCB;		}
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
		ready_not_suspended.count++;

	}
	else if (PCB->state == 1){
		//ready_suspended
		if (ready_suspended.count == 0){
			ready_suspended.head = PCB;
			ready_suspended.tail = PCB;
		}
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
		ready_suspended.count++;
	}
	else if (PCB->state == 2){
		//blocked_not_suspended
		if (blocked_not_suspended.count == 0){
			blocked_not_suspended.head = PCB;
			blocked_not_suspended.tail = PCB;

		}
		else
		{
			PCB->prev = blocked_not_suspended.tail;
			blocked_not_suspended.tail->next = PCB;
			blocked_not_suspended.tail = PCB;
		}
		blocked_not_suspended.count++;

	}
	else{
		//blocked_suspended
		if (blocked_suspended.count == 0)
		{
			blocked_suspended.head = PCB;
			blocked_suspended.tail = PCB;
		}
		else
		{
			blocked_suspended.tail->next= PCB;
			PCB->prev = blocked_suspended.tail;
			blocked_suspended.tail = PCB;
		}
		blocked_suspended.count++;
	}

}

/**
* This function is used to remove a pcb from a queue, 
* Success is printed if th epcb is removed 
* Error is printed if there was an issues removing the pcb
*
* @param  PCB: a pointer to a specific pcb
*
* 
*/
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

	if (toRemove->state == 0){
		if (ready_not_suspended.count == 1){
			ready_not_suspended.head = NULL;
			ready_not_suspended.tail = NULL;
		}
		else if (toRemove->prev == NULL){
			ready_not_suspended.head = toRemove->next;
			ready_not_suspended.head->prev = NULL;
		}
		else if (toRemove->next == NULL){
			ready_not_suspended.tail = toRemove->prev;
			ready_not_suspended.tail->next = NULL;
		}
		else{
			toRemove->prev->next = toRemove->next;
			toRemove->next->prev = toRemove->prev;
		}
	}
	else if (toRemove->state == 1){
		if (ready_suspended.count == 1){
			ready_suspended.head = NULL;
			ready_suspended.tail = NULL;
		}
		else if (toRemove->prev == NULL){
			ready_suspended.head = toRemove->next;
			ready_suspended.head->prev = NULL;
		}
		else if (toRemove->next == NULL){
			ready_suspended.tail = toRemove->prev;
			ready_suspended.tail->next = NULL;
		}
		else{
			toRemove->prev->next = toRemove->next;
			toRemove->next->prev = toRemove->prev;
		}
	}
	else if (toRemove->state == 2){
		if (blocked_not_suspended.count == 1){
			blocked_not_suspended.head = NULL;
			blocked_not_suspended.tail = NULL;
		}
		else if (toRemove->prev == NULL){
			blocked_not_suspended.head = toRemove->next;
			blocked_not_suspended.head->prev = NULL;
		}
		else if (toRemove->next == NULL){
			blocked_not_suspended.tail = toRemove->prev;
			blocked_not_suspended.tail->next = NULL;
		}
		else{
			toRemove->prev->next = toRemove->next;
			toRemove->next->prev = toRemove->prev;
		}
	}
	else {
		if (blocked_suspended.count == 1){
			blocked_suspended.head = NULL;
			blocked_suspended.tail = NULL;
		}
		else if (toRemove->prev == NULL){
			blocked_suspended.head = toRemove->next;
			blocked_suspended.head->prev = NULL;
		}
		else if (toRemove->next == NULL){
			blocked_suspended.tail = toRemove->prev;
			blocked_suspended.tail->next = NULL;
		}
		else{
			toRemove->prev->next = toRemove->next;
			toRemove->next->prev = toRemove->prev;
		}
	}
	sys_req(WRITE, DEFAULT_DEVICE, success, success_len);
}

/**
* This function is used to place a pcb in the memory that has been allocated for it
* as well as neccessary initialization. Inserts into ready not suspened queue
*
* @param  processName: a charcter pointer to what the user would like the pcb to be called
* @param  class: an integer indicating whether the pcb is an application or system process
* @param  priority: an integer indicating the priority of the pcb 

  @retval count: pointer to the pcb that has just been allocated to memory and initialized 
* 
*/
struct pcb * SetupPCB(char * processName, int class, int priority){
	struct pcb *pcb_point;
	pcb_point = AllocatePCB();
	strcpy(pcb_point->name, processName);
	
	pcb_point->class = class;
	pcb_point->priority = priority;
	pcb_point->state = 0;
	InsertPCB(pcb_point);
	return pcb_point;
}
