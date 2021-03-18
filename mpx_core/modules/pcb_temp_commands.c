#include "internal_procedures.h"
#include "structs.h"
#include "mpx_supt.h"
#include <string.h>

/**
* @brief This function will create a new PCB by calling the internal function SetupPCB
*
* @param processName: full string representation of the desired process name
* @param class: identification of the process as either a application or system process
* @param priority: the priority level of the new process for the order it is added to the process queues
*/
void CreatePCB(char *processName, int class, int priority){

	int buffer_size = 60;
	struct pcb *pcb = FindPCB(processName);

	if( pcb != NULL || priority < 0 || priority > 9 || class < 0 || class > 1 )
		sys_req(WRITE, DEFAULT_DEVICE,"\nInvalid input. Aborting createpcb command...\n", &buffer_size);
	else{
		SetupPCB(processName, class, priority);
		sys_req(WRITE, DEFAULT_DEVICE, "\nPCB was created successfully", &buffer_size);
	}
}

/**
* @brief This function will delete a PCB from the queue by calling the internal function RemovePCB
*
* @param processName: full string representation of the desired process name
*/
void DeletePCB(char *processName){

	int buffer_size = 60;
	struct pcb *pcb = FindPCB(processName);

	if(strcmp(processName, "idle") == 0 || strcmp(processName, "comhand") == 0)
    	sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:Cannot delete system processes\n", &buffer_size);
	else if(strcmp(processName, "infinite") == 0 && (pcb->state != 1 && pcb->state != 3)){
		sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:Infinite process must be suspended to delete\n", &buffer_size);
	}
	else if( pcb != NULL ){
		RemovePCB(pcb);
		FreePCB(pcb);
	}
	else
		sys_req(WRITE, DEFAULT_DEVICE,"\nUnable to find pcb. Aborting deletepcb command...\n", &buffer_size);
}

/**
* @brief This function will remove the PCB from a ready queue and add it to a blocked queue
*
* @param processName: full string representation of the desired process name
*/
void BlockPCB(char *processName){

	int buffer_size = 60;
	struct pcb *pcb = FindPCB(processName);

	if( pcb != NULL )
	{
		RemovePCB(pcb);
		if (pcb->state == 0)
			pcb->state = 2;
		else
			pcb->state = 3;
		InsertPCB(pcb);
	}
	else
		sys_req(WRITE, DEFAULT_DEVICE,"\nUnable to find pcb. Aborting blockpcb command...\n", &buffer_size);
}

/**
* @brief his function will remove the PCB from a blocked queue and add it to a ready queue
*
* @param processName: full string representation of the desired process name
*/
void UnblockPCB(char *processName){
	/*
		check if name is valid
		FindPCB(processName)
		Remove from blocked and insert into ready
	*/
	int buffer_size = 60;
	struct pcb *pcb = FindPCB(processName);

	if( pcb != NULL )
	{
		RemovePCB(pcb);
		if (pcb->state == 2)
			pcb->state = 0;
		else
			pcb->state = 1;
		InsertPCB(pcb);
	}
	else
		sys_req(WRITE, DEFAULT_DEVICE,"\nUnable to find pcb. Aborting unblockpcb command...\n", &buffer_size);
}
