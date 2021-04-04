#include "mpx_supt.h"
#include "structs.h"
#include <string.h>
#include <mem/heap.h>
#include <core/serial.h>


u32int heap_address;
struct mcb_queue free;
struct mcb_queue allocated;
u32int heapsize;

/**
* This function is used to allocate memory for a pcb
* and initializes the stack to null
*
*  @retval pcb*: returns a pcb pointer
*
*/
struct pcb * AllocatePCB(){
	int stack_size = 2048;
	struct pcb *PCB;
	PCB = sys_alloc_mem(sizeof(struct pcb));
	PCB->base = PCB->stack;
	PCB->top = PCB->base + stack_size - sizeof(struct context);
	memset(PCB->stack, 0, stack_size);
	return PCB;
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
	/*char * success = "\nSuccess!\0";
	int success_size = strlen(success);
	int *success_len = &success_size;
	char * failure = "\nERROR: The PCB could not be freed from memory\0";
	int failure_size = strlen(failure);
	int *failure_len = &failure_size;*/

	if (sys_free_mem(PCB) == -1){
		//sys_req(WRITE, DEFAULT_DEVICE, failure, failure_len);
	}
	else{
		//sys_req(WRITE, DEFAULT_DEVICE, success, success_len);
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
			ready_not_suspended.tail = PCB;
		}
		else if (PCB->priority > ready_not_suspended.head->priority){
			PCB->next = ready_not_suspended.head;
			ready_not_suspended.head->prev = PCB;
			ready_not_suspended.head = PCB;
		}
		else
		{
			struct pcb *current= ready_not_suspended.head;
			while(current->priority >= PCB->priority)
			{
				current = current->next;
			}
			PCB->prev = current->prev;
			current->prev->next = PCB;
			current->prev = PCB;
			PCB->next = current;
		}
		ready_not_suspended.count++;

	}
	else if (PCB->state == 1){
		//ready_suspended
		if (ready_suspended.count == 0){
			ready_suspended.head = PCB;
			ready_suspended.tail = PCB;
		}
		else if (ready_suspended.tail->priority >= PCB->priority)
		{
			ready_suspended.tail->next= PCB;
			PCB->prev = ready_suspended.tail;
			ready_suspended.tail = PCB;
		}
		else if (PCB->priority > ready_not_suspended.head->priority){
			PCB->next = ready_suspended.head;
			ready_suspended.head->prev = PCB;
			ready_suspended.head = PCB;
		}
		else
		{
			struct pcb *current= ready_suspended.head;
			while(current->priority >= PCB->priority)
			{
				current = current->next;
			}
			PCB->prev = current->prev;
			current->prev->next = PCB;
			current->prev = PCB;
			PCB->next = current;
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
	else if (PCB->state == 3){
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
	char * failure = "\nERROR: The PCB could not be removed\n\0";
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
		ready_not_suspended.count--;
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
		ready_suspended.count--;
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
		blocked_not_suspended.count--;
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
		blocked_suspended.count--;
	}
	toRemove->next = NULL;
	toRemove->prev = NULL;
}

/**
* This function is used to place a pcb in the memory that has been allocated for it
* as well as neccessary initialization.
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
	return pcb_point;
}

void InitializeHeap(u32int size){
	heapsize = size + sizeof(struct cmcb) + sizeof(struct lmcb);
	heap_address = kmalloc(heapsize);
	struct cmcb start_cmcb = {.beginning_address = heap_address, .type = 1, .size = heapsize, .pcb_name = "free"};
	struct cmcb *start_cmcb_ptr = &start_cmcb;
	free.head = start_cmcb_ptr;
	free.tail = start_cmcb_ptr;
	free.count = 1;
}

void AllocateMem(u32int size){
	int allocate_size = size + sizeof(struct cmcb) + sizeof(struct lmcb);
	u32int new_address;
	struct cmcb* free_mcbs = free.head;
	while (free_mcbs != NULL){
		if (free_mcbs->size >= allocate_size){
			free_mcbs->size = free_mcbs->size - allocate_size - allocated.count * (sizeof(struct cmcb) + sizeof(struct lmcb));
			new_address = free_mcbs->beginning_address;
			free_mcbs->beginning_address = new_address + allocate_size;
			struct cmcb cmcb = {.beginning_address = new_address, .type = 0, .size = allocate_size};
			struct cmcb *cmcb_ptr = &cmcb;
			if (allocated.count == 0){
				allocated.head = cmcb_ptr;
				allocated.tail = cmcb_ptr;
				allocated.count++;
			}
			else{
				if (cmcb_ptr->beginning_address < allocated.head->beginning_address){
					cmcb_ptr->next = allocated.head;
					allocated.head->prev = cmcb_ptr;
					allocated.head = cmcb_ptr;
				}
				else if (cmcb_ptr->beginning_address > allocated.tail->beginning_address){
					cmcb_ptr->prev = allocated.tail;
					allocated.tail->next = cmcb_ptr;
					allocated.tail = cmcb_ptr;
				}
				else{
					struct cmcb *allocated_curr = allocated.head->next;
					while (allocated_curr != NULL){
						if (cmcb_ptr->beginning_address < allocated_curr->beginning_address){
							cmcb_ptr->prev = allocated_curr->prev;
							allocated_curr->prev->next = cmcb_ptr;
							cmcb_ptr->next = allocated_curr;
							allocated_curr->prev = cmcb_ptr;
						}
					}
				}
			}
			break;
		}
		free_mcbs = free_mcbs->next;
	}
}

void FreeMem(u32int address){
	
	if (address == heap_address){
		if (allocated.count == 1){
			allocated.head = NULL;
			allocated.tail = NULL;
			free.head->beginning_address = address;
			free.head->size = heapsize;
		}
		else{
			struct cmcb *freed_mem = allocated.head;
			//allocated.head->type = 1;
			allocated.head = freed_mem->next;
			freed_mem->next = NULL;
			allocated.head->prev = NULL;
			if (free.head->beginning_address == (heap_address + freed_mem->size)){
				free.head->beginning_address = address;
				free.head->size += freed_mem->size;
			}
			else{
				freed_mem->type = 1;
				strcpy(freed_mem->pcb_name, "free");
				free.head->prev = freed_mem;
				freed_mem->next = free.head;
				free.head = freed_mem;
			}
		}
	}
	else if (address == allocated.tail->beginning_address){
		struct cmcb *tail_mcb = allocated.tail;
		allocated.tail = tail_mcb->prev;
		tail_mcb->prev = NULL;
		allocated.tail->next = NULL;
		if (free.tail->beginning_address == (address - tail_mcb->size)){
			free.tail->size += tail_mcb->size;
		}
		else if (free.tail->beginning_address == (address + tail_mcb->size)){
			free.tail->beginning_address = address;
			free.tail->size += tail_mcb->size;
		}
		else{
			tail_mcb->type = 1;
			strcpy(tail_mcb->pcb_name, "free");
			tail_mcb->prev = free.tail;
			free.tail->next = tail_mcb;
			free.tail = tail_mcb;
		}
	}
	struct cmcb *alloc_mcbs = allocated.head;
	while (alloc_mcbs != NULL){
		if (alloc_mcbs->beginning_address == address){
			struct cmcb *selected = alloc_mcbs;
			selected->prev->next = selected->next;
			selected->next->prev = selected->prev;
			struct cmcb *free_mcbs = free.head;
			while (address >= free_mcbs->beginning_address){
				free_mcbs = free_mcbs->next;
			}
			if (free_mcbs->beginning_address == (address - selected->size)){
				free_mcbs->size += selected->size;
			}
			else if (free_mcbs->beginning_address == (address + selected->size)){
				free_mcbs->beginning_address = address;
				free_mcbs->size += selected->size;
			}
			else{
				selected->type = 1;
				strcpy(selected->pcb_name, "free");
				free_mcbs->prev->next = selected;
				selected->prev = free_mcbs->prev;
				free_mcbs->prev = selected;
				selected->next = free_mcbs;
			}
			break;
		}
		alloc_mcbs = alloc_mcbs->next;
	}
}

int isEmpty(){
	if (allocated.head == NULL){
		return 1;
	}
	else{
		return 0;
	}
}