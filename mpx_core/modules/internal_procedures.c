#include "mpx_supt.h"
#include "structs.h"
#include <string.h>
#include <mem/heap.h>
#include <core/serial.h>


struct cmcb_queue free;
struct cmcb_queue allocated;
u32int heapsize;

/**
* This function is used to allocate memory for a pcb
* and initializes the stack to null
*
*  @retval pcb*: returns a pcb pointer
*
*/
struct pcb * AllocatePCB(){
	char error[30] = "\nPCB could not be allocated\n";
	int error_size = 30;
	int stack_size = 2048;
	struct pcb *PCB;
	PCB = sys_alloc_mem(sizeof(struct pcb));
	if (PCB == NULL){
		sys_req(WRITE, DEFAULT_DEVICE, error, &error_size);
	}
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
	char success[32] = "\nSuccess! PCB freed from memory";
	int success_size = strlen(success);
	char failure[46] = "\nERROR: The PCB could not be freed from memory";
	int failure_size = strlen(failure);

	if (sys_free_mem(PCB) == -1){
		sys_req(WRITE, DEFAULT_DEVICE, failure, &failure_size);
	}
	else{
		sys_req(WRITE, DEFAULT_DEVICE, success, &success_size);
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
	if (pcb_point == NULL){
		return NULL;
	}
	strcpy(pcb_point->name, processName);
	pcb_point->class = class;
	pcb_point->priority = priority;
	pcb_point->state = 0;
	return pcb_point;
}

void InitializeHeap(u32int size){
	heapsize = size + sizeof(struct cmcb);
	heap_address = kmalloc(heapsize);
	struct cmcb *start_cmcb_ptr = (struct cmcb *)heap_address;
	start_cmcb_ptr->beginning_address = heap_address + sizeof(struct cmcb);
	start_cmcb_ptr->type = 1;
	start_cmcb_ptr->size = size;
	start_cmcb_ptr->next = NULL;
	start_cmcb_ptr->prev = NULL;
	allocated.head = NULL;
	allocated.tail = NULL;
	free.head = start_cmcb_ptr;
	free.tail = start_cmcb_ptr;
	free.count = 1;
}

u32int AllocateMem(u32int size){
	u32int allocate_size = size + sizeof(struct cmcb);
	u32int new_address;
	u32int old_size;
	struct cmcb* free_mcbs = free.head;
	while (free_mcbs != NULL){
		if (free_mcbs->size == (int)size){
			new_address = free_mcbs->beginning_address;
			if (free_mcbs == free.head && free_mcbs == free.tail){
				free.head = NULL;
				free.tail = NULL;
			}
			else if (free_mcbs == free.head){
				free.head = free.head->next;
				free.head->prev = NULL;
				free_mcbs->next = NULL;
			}
			else if (free_mcbs == free.tail){
				free.tail = free.tail->prev;
				free.tail->next = NULL;
				free_mcbs->prev = NULL;
			}
			else{
				free_mcbs->next->prev = free_mcbs->prev;
				free_mcbs->prev->next = free_mcbs->next;
				free_mcbs->next = NULL;
				free_mcbs->prev = NULL;
			}
			struct cmcb *cmcb = (struct cmcb*)new_address;
			cmcb->beginning_address = new_address;
			cmcb->type = 0;
			cmcb->size = size;
			cmcb->next = NULL;
			cmcb->prev = NULL;
			if (allocated.count == 0){
				allocated.head = cmcb;
				allocated.tail = cmcb;
				allocated.count++;
			}
			else{
				if (cmcb->beginning_address < allocated.head->beginning_address){
					cmcb->next = allocated.head;
					allocated.head->prev = cmcb;
					allocated.head = cmcb;

					if (allocated.count == 1){
						allocated.tail->prev = allocated.head;
					}

					allocated.count++;
				}
				else if (cmcb->beginning_address > allocated.tail->beginning_address){
					cmcb->prev = allocated.tail;
					allocated.tail->next = cmcb;
					allocated.tail = cmcb;

					if (allocated.count == 1){
						allocated.head->next = allocated.tail;
					}
					allocated.count++;
				}
				else{
					struct cmcb *allocated_curr = allocated.head->next;
					while (allocated_curr != NULL){
						if (cmcb->beginning_address < allocated_curr->beginning_address){
							cmcb->prev = allocated_curr->prev;
							allocated_curr->prev->next = cmcb;
							cmcb->next = allocated_curr;
							allocated_curr->prev = cmcb;
							allocated.count++;
						}
					}
				}
			}
			return new_address;
		}
		else if ((u32int)free_mcbs->size >= (size + sizeof(struct cmcb))){

			new_address = free_mcbs->beginning_address;
			old_size = free_mcbs->size;
			free_mcbs = (struct cmcb*)(free_mcbs + allocate_size);
			free_mcbs->size = old_size - allocate_size;
			free_mcbs->beginning_address = new_address + allocate_size;
			free_mcbs->next = NULL;
			free_mcbs->prev = NULL;

			struct cmcb * free_curr = free.head;
			while (free_curr != NULL){
				if (free_curr->beginning_address == new_address){
					if (free_curr == free.head && free_curr == free.tail){
						free.head = free_mcbs;
						free.tail = free_mcbs;
						break;
					}
					else if (free_curr == free.head){
						free_mcbs->next = free_curr->next;
						free_curr->next->prev = free_mcbs;
						free.head = free_mcbs;
						free_curr->next = NULL;
						break;
					}
					else if (free_curr == free.tail){
						free_mcbs->prev = free_curr->prev;
						free_curr->prev->next = free_mcbs;
						free.tail = free_mcbs;
						free_curr->prev = NULL;
						break;
					}
					else{
						free_curr->prev->next = free_mcbs;
						free_curr->next->prev = free_mcbs;
						free_curr->next = NULL;
						free_curr->next = NULL;
						break;
					}
				}
				free_curr = free_curr->next;
			}

			struct cmcb *cmcb = (struct cmcb*)(new_address - sizeof(struct cmcb));
			cmcb->beginning_address = new_address;
			cmcb->type = 0;
			cmcb->size = size;
			cmcb->next = NULL;
			cmcb->prev = NULL;
			if (allocated.count == 0){
				allocated.head = cmcb;
				allocated.tail = cmcb;
				allocated.count++;
			}
			else{
				if (cmcb->beginning_address < allocated.head->beginning_address){
					cmcb->next = allocated.head;
					allocated.head->prev = cmcb;
					allocated.head = cmcb;

					if (allocated.count == 1){
						allocated.tail->prev = allocated.head;
					}

					allocated.count++;
				}
				else if (cmcb->beginning_address > allocated.tail->beginning_address){
					cmcb->prev = allocated.tail;
					allocated.tail->next = cmcb;
					allocated.tail = cmcb;

					if (allocated.count == 1){
						allocated.head->next = allocated.tail;
					}
					allocated.count++;
				}
				else{
					struct cmcb *allocated_curr = allocated.head->next;
					while (allocated_curr != NULL){
						if (cmcb->beginning_address < allocated_curr->beginning_address){
							cmcb->prev = allocated_curr->prev;
							allocated_curr->prev->next = cmcb;
							cmcb->next = allocated_curr;
							allocated_curr->prev = cmcb;
							allocated.count++;
						}
						allocated_curr = allocated_curr->next;
					}
				}
			}
			return new_address;
		}
		free_mcbs = free_mcbs->next;
	}
	return NULL;
}

int FreeMem(void* address){

	struct cmcb *alloc_mcbs = allocated.head;
	while (alloc_mcbs != NULL){
		if (alloc_mcbs->beginning_address == (u32int)address){
			if ((u32int)address == heap_address + sizeof(struct cmcb)){
				if (allocated.count == 1){
					if (free.head == NULL){
						free.head = allocated.head;
						free.head->next = NULL;
						free.head->prev = NULL;
						free.head->type = 1;
						free.tail = free.head;
					}
					allocated.head = NULL;
					allocated.tail = NULL;
					alloc_mcbs->next = NULL;
					alloc_mcbs->prev = NULL;
					free.head = (struct cmcb*)(heapsize);
					free.head->beginning_address = (u32int)address;
					free.head->size = heapsize - sizeof(struct cmcb);
					free.head->next = NULL;
					free.head->prev = NULL;
				}
				else{
					allocated.head = alloc_mcbs->next;
					alloc_mcbs->next = NULL;
					allocated.head->prev = NULL;
					if (free.head->beginning_address == ((u32int)address + alloc_mcbs->size + sizeof(struct cmcb))){
						free.head->beginning_address = (u32int)address;
						free.head->size += alloc_mcbs->size + sizeof(struct cmcb);
						free.head = (struct cmcb *)((u32int)address - sizeof(struct cmcb));
					}
					else{
						alloc_mcbs->type = 1;
						free.head->prev = alloc_mcbs;
						alloc_mcbs->next = free.head;
						free.head = alloc_mcbs;
					}
				}
				allocated.count--;
				return 0;
			}
			else if (allocated.count == 1){
				allocated.head = NULL;
				allocated.tail = NULL;
				free.head->size = heapsize - sizeof(struct cmcb);
				free.tail = free.head;
				free.tail->prev = NULL;
				free.head->next = NULL;
				allocated.count--;
				
				return 0;
			}
			else if ((u32int)address == allocated.tail->beginning_address){
				allocated.tail = alloc_mcbs->prev;
				alloc_mcbs->prev = NULL;
				allocated.tail->next = NULL;
				if (free.tail->beginning_address == ((u32int)address - free.tail->size - sizeof(struct cmcb))){
					free.tail->size += alloc_mcbs->size + sizeof(struct cmcb);
				}
				else if (free.tail->beginning_address == ((u32int)address + alloc_mcbs->size + sizeof(struct cmcb))){
					free.tail->beginning_address = (u32int)address;
					free.tail->size += alloc_mcbs->size + sizeof(struct cmcb);
					if (free.tail->prev->beginning_address == ((u32int)address - free.tail->prev->size - sizeof(struct cmcb))){
						free.tail->prev->size += free.tail->size + sizeof(struct cmcb);
						free.tail = free.tail->prev;
						free.tail->next->prev = NULL;
						free.tail->next = NULL;
					}
				}
				else{
					alloc_mcbs->type = 1;
					alloc_mcbs->prev = free.tail;
					free.tail->next = alloc_mcbs;
					free.tail = alloc_mcbs;
				}
				allocated.count--;
				
				return 0;
			}
			else if ((u32int)address == allocated.head->beginning_address){
				allocated.head = alloc_mcbs->next;
				alloc_mcbs->next = NULL;
				allocated.head->prev = NULL;
				if (free.head->beginning_address == ((u32int)address - free.head->size - sizeof(struct cmcb))){
					free.head->size += alloc_mcbs->size + sizeof(struct cmcb);
					if (free.head->next->beginning_address == ((u32int)address + alloc_mcbs->size + sizeof(struct cmcb))){
						free.head->size += free.head->next->size + sizeof(struct cmcb);
						free.head->next->prev = NULL;
						free.head->next = free.head->next->next;
						free.head->next->prev = free.head;
					}
				}
				else if (free.head->beginning_address == ((u32int)address + alloc_mcbs->size + sizeof(struct cmcb))){
					free.head->beginning_address = (u32int)address;
					free.head->size += alloc_mcbs->size + sizeof(struct cmcb);
					free.head = (struct cmcb *)((u32int)address - sizeof(struct cmcb));
				}
				else{
					alloc_mcbs->type = 1;
					alloc_mcbs->next = free.head;
					free.head->prev = alloc_mcbs;
					free.head = alloc_mcbs;
				}
				allocated.count--;
				
				return 0;
			}
			else {
				alloc_mcbs->prev->next = alloc_mcbs->next;
				alloc_mcbs->next->prev = alloc_mcbs->prev;
				alloc_mcbs->next = NULL;
				alloc_mcbs->prev = NULL;
				struct cmcb *free_mcbs = free.head;
				if ((u32int)address < free_mcbs->beginning_address){
					if (free_mcbs->beginning_address == ((u32int)address + alloc_mcbs->size + sizeof(struct cmcb))){
						free_mcbs->beginning_address = (u32int)address;
						free_mcbs->size += alloc_mcbs->size + sizeof(struct cmcb);
					}
					else {
						alloc_mcbs->type = 1;
						alloc_mcbs->next = free_mcbs;
						free_mcbs->prev = alloc_mcbs;
						free.head = alloc_mcbs;
					}
				}
				else{
					while ((u32int)address > free_mcbs->beginning_address){
						free_mcbs = free_mcbs->next;
					}

					if (free_mcbs->prev->beginning_address == ((u32int)address - free_mcbs->prev->size - sizeof(struct cmcb))){
						free_mcbs->prev->size += alloc_mcbs->size + sizeof(struct cmcb);
						if (free_mcbs->beginning_address == ((u32int)address + alloc_mcbs->size + sizeof(struct cmcb))){
							free_mcbs->prev->size += free_mcbs->size + sizeof(struct cmcb);
							free_mcbs->prev->next = free_mcbs->next;
							free_mcbs->next->prev = free_mcbs->prev;
							free_mcbs->next = NULL;
							free_mcbs->prev = NULL;
						}
					}
					else if (free_mcbs->beginning_address == ((u32int)address + alloc_mcbs->size + sizeof(struct cmcb))){
						free_mcbs->beginning_address = (u32int)address;
						free_mcbs->size += alloc_mcbs->size + sizeof(struct cmcb);
					}
					else{
						alloc_mcbs->type = 1;
						free_mcbs->prev->next = alloc_mcbs;
						alloc_mcbs->prev = free_mcbs->prev;
						free_mcbs->prev = alloc_mcbs;
						alloc_mcbs->next = free_mcbs;
					}
				}
				allocated.count--;
				return 0;
			}
		}
		alloc_mcbs = alloc_mcbs->next;
	}
	return -1;
}

/**
*	@brief This function returns a boolean
*		   value indicating whether the allocated
*		   list has any elements.
*	@param none
*	@retval 1 if allocated memory blocks exist in the queue
*	@retval 0 if allocated memory queue is empty
*/
int isEmpty(){
	if (allocated.head == NULL){
		return 1;
	}
	else{
		return 0;
	}
}

/**
*	@brief This function interates through the list of
*		   free memory blocks and displays the size
*		   of the given block as well as its address
*		   within the heap
*	@param none
*	@retval none
*/
void showFree(){
	struct cmcb *cur = free.head;
	char addr[10] = "";
	char size[6] = "";
	char msg[14]= "\nFree Block: \0";
	char size_msg[8] = "Size: \0";
	char address[11] = "Address: \0";
	char newline[2] = "\n";
	int address_size = strlen(address);
	int size_msg_size = 8;
	int msg_size = strlen(msg);
	int new_line = 1;

	char empty_free[34] = "\nThere are no free blocks.\n";
	int empty_free_size = strlen(empty_free);
	if(cur == NULL)
		sys_req(WRITE, DEFAULT_DEVICE, empty_free, &empty_free_size);

	while(cur != NULL)
	{
		sys_req(WRITE, DEFAULT_DEVICE, msg, &msg_size);
		sys_req(WRITE, DEFAULT_DEVICE, newline, &new_line);
		sys_req(WRITE, DEFAULT_DEVICE, size_msg, &size_msg_size);
		itoa(cur->size, size, 10);
		int size_size = strlen(size);
		sys_req(WRITE, DEFAULT_DEVICE, size, &size_size);
		sys_req(WRITE, DEFAULT_DEVICE, newline, &new_line);
		sys_req(WRITE, DEFAULT_DEVICE, address, &address_size);
		itoa((int)cur->beginning_address, addr, 10);
		int addr_size = strlen(addr);
		sys_req(WRITE, DEFAULT_DEVICE, addr, &addr_size);
		sys_req(WRITE, DEFAULT_DEVICE, newline, &new_line);

		cur = cur->next;
	}
}

/**
*	@brief This function interates through the list of
*		   allocated memory blocks and displays the size
*		   of the given block as well as its address
*		   within the heap
*	@param none
*	@retval none
*/
void showAllocated(){
	struct cmcb *cur = allocated.head;
	char addr[10] = "";
	char size[5] = "";
	char msg[20]= "\nAllocated Block:\n";
	char size_msg[8] = "Size: \0";
	char address[11] = "Address: \0";
	char newline[2] = "\n";
	int address_size = strlen(address);
	int size_msg_size = 8;
	int msg_size = strlen(msg);
	int new_line = 1;

	char empty_malloc[32] = "\nThere are no allocated blocks.\n";
	int empty_malloc_size = strlen(empty_malloc);
	if(cur == NULL)
		sys_req(WRITE, DEFAULT_DEVICE, empty_malloc, &empty_malloc_size);

	while(cur != NULL)
	{
		sys_req(WRITE, DEFAULT_DEVICE, msg, &msg_size);
		sys_req(WRITE, DEFAULT_DEVICE, size_msg, &size_msg_size);
		itoa(cur->size, size, 10);
		int size_size = strlen(size);
		sys_req(WRITE, DEFAULT_DEVICE, size, &size_size);
		sys_req(WRITE, DEFAULT_DEVICE, newline, &new_line);
		itoa((int)cur->beginning_address, addr, 10);
		int addr_size = strlen(addr);
		sys_req(WRITE, DEFAULT_DEVICE, address, &address_size);
		sys_req(WRITE, DEFAULT_DEVICE, addr, &addr_size);
		sys_req(WRITE, DEFAULT_DEVICE, newline, &new_line);

		cur = cur->next;
	}
}