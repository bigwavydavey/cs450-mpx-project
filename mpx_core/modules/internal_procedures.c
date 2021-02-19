#include "mpx_supt.h"

PCB * AllocatePCB(){
	/*
		sys_alloc_mem();
	*/
	return NULL;
}

PCB * FindPCB(char *processName){
	/*
		search queues for PCB with processName
		return that PCB pointer
	*/
	return NULL;
}

char * FreePCB(PCB *pcb){
	/*
		sys_free_mem();
	*/
}

void InsertPCB(PCB *pcb){
/*
	if ready
		search queue for priority, then insert at end of priority
	else
		put on tail of queue
*/
}

char * RemovePCB(PCB *pcb){
	FindPCB(pcb.processName);
	//remove from queue
	//return success or error code
}

PCB * SetupPCB(char * processName, int class, int priority){
	ALlocatePCB();
}