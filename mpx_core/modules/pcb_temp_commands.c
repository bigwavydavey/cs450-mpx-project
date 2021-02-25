#include "internal_procedures.h"
#include "structs.h"
#include "mpx_supt.h"

void CreatePCB(char *processName, int class, int priority){

	int buffer_size = 60;
	struct pcb *pcb = FindPCB(processName);

	if( pcb != NULL || priority < 0 || priority > 9 || class < 0 || class > 1 )
		sys_req(WRITE, DEFAULT_DEVICE,"\nInvalid input. Aborting createpcb command...\n", &buffer_size);
	else
		SetupPCB(processName, class, priority);
}

void DeletePCB(char *processName){

	int buffer_size = 60;
	struct pcb *pcb = FindPCB(processName);

	if( pcb != NULL ){
		RemovePCB(pcb);
		FreePCB(pcb);
	}
	else
		sys_req(WRITE, DEFAULT_DEVICE,"\nUnable to find pcb. Aborting deletepcb command...\n", &buffer_size);
}

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
