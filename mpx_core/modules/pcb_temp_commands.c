#include "SetupPCB.h"
#include "InsertPCB.h"
#include "RemovePCB.h"
#include "FreePCB.h"
#include "AllocatePCB.h"
#include "FindPCB.h"

void CreatePCB(char *processName, int class, int priority){
	/*
		check if name is unique(use FindPCB(processName)) and valid
		check class
		check priority must be 0-9
		Create PCB pcb
		SetupPCB(processName, class, priority);
		InsertPCB(*pcb);
	*/
}

void DeletePCB(char *processName){
	/*
		check if name is valid
		RemovePCB()
		FreePCB()
	*/
}

void BlockPCB(char *processName){
	/*
		check if name is valid
		FindPCB(processName)
		Remove from ready and insert into blocked
	*/
}
void UnblockPCB(char *processName){
	/*
		check if name is valid
		FindPCB(processName)
		Remove from blocked and insert into ready
	*/
}