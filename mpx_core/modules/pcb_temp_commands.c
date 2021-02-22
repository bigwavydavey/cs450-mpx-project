#include "internal_procedures.h"

void CreatePCB(char *processName, int class, int priority){

	SetupPCB(processName, class, priority);
}

void DeletePCB(char *processName){
	
	RemovePCB(FindPCB(processName));
}

void BlockPCB(char *processName){
	
	RemovePCB(FindPCB(processName));
}
void UnblockPCB(char *processName){
	/*
		check if name is valid
		FindPCB(processName)
		Remove from blocked and insert into ready
	*/
	RemovePCB(FindPCB(processName));
}