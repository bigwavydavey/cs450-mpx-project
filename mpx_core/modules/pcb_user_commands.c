#include <string.h>
#include "internal_procedures.h"
#include "mpx_supt.h"
#include "structs.h"
int buffer_length = 99;
char input[1];
/**
  @brief This function changes the state
         of a user selected PCB to suspended
         and inserts it into the correct
         queue
  
  @param processName: name of PCB to alter

  @retval none
*/
void SuspendPCB(char *processName){
	/*
		check name
		FindPCB(processName);
		if 4 queues, remove from non-suspeneded and
		insert into suspended
	*/
  struct pcb *pcb = FindPCB(processName);
  if(pcb == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:The pcb you entered does not exist\n", &buffer_length);
  else
  {
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
}

/**
  @brief This function changes the state
         of a user selected PCB to unsuspended
         and inserts it into the correct
         queue
  
  @param processName: name of PCB to alter

  @retval none
*/
void ResumePCB(char *processName){
	/*
		check name
		FindPCB(processName);
		if 4 queues, remove from suspeneded and
		insert into non-suspended
	*/
  struct pcb *pcb = FindPCB(processName);

  if(pcb == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:The pcb you entered does not exist\n", &buffer_length);
  else
  {
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
}

/**
  @brief This function displays a user
         selected PCB to the terminal
  
  @param processName: name of PCB to alter
  @param priority: new value to set as PCB priority

  @retval none
*/
void SetPCBPriority(char *processName, int priority){
	/*
		check name and priority
		struct PCB *pcb = FindPCB(*processName);
		pcb.priority = priority;
	*/
  struct pcb *pcb = FindPCB(processName);
  if(pcb == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:The pcb you entered does not exist\n", &buffer_length);
  else
  {
    pcb->priority = priority;
  }
}
/**
  @brief This function displays a user
         selected PCB to the terminal
  
  @param processName: name of PCB to display

  @retval none
*/
void ShowPCB(char *processName)
{
	/*
		check name
		FindPCB(processName);
		buffer = "Process Name: processName | State: state | Suspended Status: status | Priority: priority\n";
		sys_req(WRITE, DEFAULT_DEVICE, buffer, buffer.len());
	*/
  struct pcb *display_pcb = FindPCB(processName);

  if (display_pcb->name == NULL)
  {
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:The pcb you entered does not exist\n", &buffer_length);
  }
  else
  {
  	sys_req(WRITE, DEFAULT_DEVICE, "\nProcess Name: ", &buffer_length);
    sys_req(WRITE, DEFAULT_DEVICE, display_pcb->name, &buffer_length);
    sys_req(WRITE, DEFAULT_DEVICE, "\nClass: ", &buffer_length);
    itoa(display_pcb->class, input, 10);
    sys_req(WRITE, DEFAULT_DEVICE, input, &buffer_length);
    sys_req(WRITE, DEFAULT_DEVICE, "\nPriority: ", &buffer_length);
    itoa(display_pcb->priority, input, 10);
    sys_req(WRITE, DEFAULT_DEVICE, input, &buffer_length);
    itoa(display_pcb->state, input, 10);

    sys_req(WRITE, DEFAULT_DEVICE, "\nState: ", &buffer_length);
    sys_req(WRITE, DEFAULT_DEVICE, input, &buffer_length);

    sys_req(WRITE, DEFAULT_DEVICE, "\nSuspended status: ", &buffer_length);
    if (display_pcb->state == 0)
      sys_req(WRITE, DEFAULT_DEVICE, "ready, not suspended", &buffer_length);
    else if (display_pcb->state == 1)
      sys_req(WRITE, DEFAULT_DEVICE, "ready, suspended", &buffer_length);
    else if (display_pcb->state == 2)
      sys_req(WRITE, DEFAULT_DEVICE, "blocked, not suspended", &buffer_length);
    else if (display_pcb->state == 3)
      sys_req(WRITE, DEFAULT_DEVICE, "blocked, suspended", &buffer_length);
    else if (display_pcb->state == 5)
      sys_req(WRITE, DEFAULT_DEVICE, "running", &buffer_length);
    else
      sys_req(WRITE, DEFAULT_DEVICE, "UNKNOWN", &buffer_length);
  }
}

/**
  @brief This function displays all currently
         ready PCBs
  @param none

  @retval none
*/
void ShowReady(){
	/*
		struct PCB pcb = ReadyQueueHead
		sys_req(WRITE, DEFAULT_DEVICE, "Ready:\n", count);
		while pcb.next != NULL
		ShowPCB(pcb.processName);
	*/
  sys_req(WRITE, DEFAULT_DEVICE, "\nReady:", &buffer_length);
  struct pcb *pcb;
  if(ready_suspended.head == NULL && ready_not_suspended.head == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "There are no ready processes.", &buffer_length);
  else if(ready_suspended.head == NULL){
    pcb = ready_not_suspended.head;
    while( pcb != NULL )
    {
      ShowPCB(pcb->name);
      pcb = pcb->next;
    }
  }
  else if (ready_not_suspended.head == NULL){
    pcb = ready_suspended.head;
    while( pcb != NULL )
    {
      ShowPCB(pcb->name);
      pcb = pcb->next;
    }
  }
  else{
    pcb = ready_suspended.head;
    while( pcb != NULL )
    {
      ShowPCB(pcb->name);
      pcb = pcb->next;
    }
    pcb = ready_not_suspended.head;
    while( pcb != NULL )
    {
      ShowPCB(pcb->name);
      pcb = pcb->next;
    }
  }
}

/**
  @brief This function displays all currently
         blocked PCBs
  @param none

  @retval none
*/
void ShowBlocked(){
	/*
		struct PCB pcb = BlockedQueueHead
		sys_req(WRITE, DEFAULT_DEVICE, "Blocked:\n", count);
		while pcb.next != NUll
		ShowPCB(pcb.processName);
	*/
  struct pcb *pcb = blocked_suspended.head;
  sys_req(WRITE, DEFAULT_DEVICE, "\nBlocked:", &buffer_length);

  if(blocked_suspended.head == NULL && blocked_not_suspended.head == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "There are no blocked processes.", &buffer_length);
  else if(blocked_suspended.head == NULL){
   	pcb = blocked_not_suspended.head;
   	while( pcb != NULL ) 	{
      	ShowPCB(pcb->name);
      	pcb = pcb->next;
    }
  }
  else if (blocked_not_suspended.head == NULL){
    pcb = blocked_suspended.head;
    while( pcb != NULL )
    {
      ShowPCB(pcb->name);
      pcb = pcb->next;
    }
  }
  else{
    pcb = blocked_suspended.head;
    while( pcb != NULL )
    {
      ShowPCB(pcb->name);
      pcb = pcb->next;
    }
    pcb = blocked_not_suspended.head;
    while( pcb != NULL )
    {
      ShowPCB(pcb->name);
      pcb = pcb->next;
    }
  }
}
/**
  @brief This function combines the ShowReady()
         function and the ShowBlocked() function
         to display all existing PCBS
  @param none

  @retval none
*/
void ShowAll(){
  ShowReady();
  ShowBlocked();
}
