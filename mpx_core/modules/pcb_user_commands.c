#include <string.h>
#include "internal_procedures.h"
#include "mpx_supt.h"
#include "structs.h"

void SuspendPCB(char *processName){
	/*
		check name
		FindPCB(processName);
		if 4 queues, remove from non-suspeneded and
		insert into suspended
	*/
  struct pcb *pcb = FindPCB(processName);
  if( pcb->state == 0 )
  {
    RemovePCB(pcb);
    pcb->state = 1;
    InsertPCB(pcb);
  }
  else if( pcb->state == 2 )
  {
    RemovePCB(pcb);
    pcb->state = 3;
    InsertPCB(pcb);
  }
}

void ResumePCB(char *processName){
	/*
		check name
		FindPCB(processName);
		if 4 queues, remove from suspeneded and
		insert into non-suspended
	*/
  struct pcb *pcb = FindPCB(processName);
  if( pcb->state == 1 )
  {
    RemovePCB(pcb);
    pcb->state = 0;
    InsertPCB(pcb);
  }
  else if( pcb->state == 3 )
  {
    RemovePCB(pcb);
    pcb->state = 2;
    InsertPCB(pcb);
  }
}

void SetPCBPriority(char *processName, int priority){
	/*
		check name and priority
		struct PCB *pcb = FindPCB(*processName);
		pcb.priority = priority;
	*/
  struct pcb *pcb = FindPCB(processName);
  pcb->priority = priority;
}

void ShowPCB(char *processName){
	/*
		check name
		FindPCB(processName);
		buffer = "Process Name: processName | State: state | Suspended Status: status | Priority: priority\n";
		sys_req(WRITE, DEFAULT_DEVICE, buffer, buffer.len());
	*/
  FindPCB(processName);
  char *buffer = "Process Name: processName | State: state | Suspended Status: status | Priority: priority\n";
  int buffer_length = strlen(buffer);
  sys_req(WRITE, DEFAULT_DEVICE, buffer, &buffer_length);
}

void ShowReady(){
	/*
		struct PCB pcb = ReadyQueueHead
		sys_req(WRITE, DEFAULT_DEVICE, "Ready:\n", count);
		while pcb.next != NULL
		ShowPCB(pcb.processName);
	*/
  struct pcb *pcb = ready_suspended->head;
  int length = 10;
  sys_req(WRITE, DEFAULT_DEVICE, "Ready:\n", &length);

  while( pcb->next != NULL )
  {
    ShowPCB(pcb->name);
  }

  struct pcb *pcb = ready_not_suspended->head;
  while( pcb->next != NULL )
  {
    ShowPCB(pcb->name);
  }
}

void ShowBlocked(){
	/*
		struct PCB pcb = BlockedQueueHead
		sys_req(WRITE, DEFAULT_DEVICE, "Blocked:\n", count);
		while pcb.next != NUll
		ShowPCB(pcb.processName);
	*/
  struct pcb *pcb = blocked_suspended->head;
  int length = 10;
  sys_req(WRITE, DEFAULT_DEVICE, "Blocked:\n", &length);

  while( pcb->next != NULL )
  {
    ShowPCB(pcb->name);
  }

  struct pcb *pcb = blocked_not_suspended->head;
  while( pcb->next != NULL )
  {
    ShowPCB(pcb->name);
  }
}

void ShowAll(){
	/*
		ShowReady();
		ShowBlocked();
	*/
  ShowReady();
  ShowBlocked();
}
