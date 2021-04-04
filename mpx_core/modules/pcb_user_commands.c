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

  struct pcb *pcb = FindPCB(processName);
  if(pcb == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:The pcb you entered does not exist\n", &buffer_length);
  else if(strcmp(processName, "idle") == 0 || strcmp(processName, "comhand") == 0 || strcmp(processName, "alarm") == 0)
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:Cannot suspend system processes\n", &buffer_length);
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

  struct pcb *pcb = FindPCB(processName);

  if(pcb == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:The pcb you entered does not exist\n", &buffer_length);
  else if(strcmp(processName, "idle") == 0 || strcmp(processName, "comhand") == 0)
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:Cannot resume system processes\n", &buffer_length);
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

  struct pcb *pcb = FindPCB(processName);
  if(pcb == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:The pcb you entered does not exist\n", &buffer_length);
  else if(strcmp(processName, "idle") == 0 || strcmp(processName, "comhand") == 0)
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:Cannot edit system processes\n", &buffer_length);
  else
  {
    if (priority < 0 || priority > 9)
      sys_req(WRITE, DEFAULT_DEVICE, "\nERROR:The priority value is invalid\n", &buffer_length);
    else
      pcb->priority = priority;
      RemovePCB(pcb);
      InsertPCB(pcb);
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
      sys_req(WRITE, DEFAULT_DEVICE, "ready, not suspended\n", &buffer_length);
    else if (display_pcb->state == 1)
      sys_req(WRITE, DEFAULT_DEVICE, "ready, suspended\n", &buffer_length);
    else if (display_pcb->state == 2)
      sys_req(WRITE, DEFAULT_DEVICE, "blocked, not suspended\n", &buffer_length);
    else if (display_pcb->state == 3)
      sys_req(WRITE, DEFAULT_DEVICE, "blocked, suspended\n", &buffer_length);
    else if (display_pcb->state == 5)
      sys_req(WRITE, DEFAULT_DEVICE, "running\n", &buffer_length);
    else
      sys_req(WRITE, DEFAULT_DEVICE, "UNKNOWN\n", &buffer_length);
  }
}

/**
  @brief This function displays all currently
         ready PCBs
  @param none

  @retval none
*/
void ShowReady(){

  sys_req(WRITE, DEFAULT_DEVICE, "\nReady:\n", &buffer_length);
  struct pcb *pcb;
  if(ready_suspended.head == NULL && ready_not_suspended.head == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "There are no ready processes.\n", &buffer_length);
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
  struct pcb *pcb;
  sys_req(WRITE, DEFAULT_DEVICE, "\nBlocked:\n", &buffer_length);

  if(blocked_suspended.head == NULL && blocked_not_suspended.head == NULL)
    sys_req(WRITE, DEFAULT_DEVICE, "There are no blocked processes.\n", &buffer_length);
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
