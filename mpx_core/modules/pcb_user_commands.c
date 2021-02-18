#include "SetupPCB.h"
#include "InsertPCB.h"
#include "RemovePCB.h"
#include "FreePCB.h"
#include "AllocatePCB.h"
#include "FindPCB.h"

void SuspendPCB(char *processName){
	/*
		check name
		FindPCB(processName);
		if 4 queues, remove from non-suspeneded and
		insert into suspended
	*/
}

void ResumePCB(char *processName){
	/*
		check name
		FindPCB(processName);
		if 4 queues, remove from suspeneded and
		insert into non-suspended
	*/
}

void SetPCBPriority(char *processName, int priority){
	/*
		check name and priority
		struct PCB *pcb = FindPCB(*processName);
		pcb.priority = priority;
	*/
}

void ShowPCB(char *processName){
	/*
		check name
		FindPCB(processName);
		buffer = "Process Name: processName | State: state | Suspended Status: status | Priority: priority\n";
		sys_req(WRITE, DEFAULT_DEVICE, buffer, buffer.len());
	*/
}

void ShowReady(){
	/*
		struct PCB pcb = ReadyQueueHead
		sys_req(WRITE, DEFAULT_DEVICE, "Ready:\n", count);
		while pcb.next != NULL
		ShowPCB(pcb.processName);
	*/
}

void ShowBlocked(){
	/*
		struct PCB pcb = BlockedQueueHead
		sys_req(WRITE, DEFAULT_DEVICE, "Blocked:\n", count);
		while pcb.next != NUll
		ShowPCB(pcb.processName);
	*/
}

void ShowAll(){
	/*
		ShowReady();
		ShowBlocked();
	*/
}