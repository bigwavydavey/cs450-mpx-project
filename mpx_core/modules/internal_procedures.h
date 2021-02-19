#ifndef _INTERNAL_PROCEDURES_H
#define _INTERNAL_PROCEDURES_H

PCB * AllocatePCB();
PCB * FindPCB(char *processName);
char * FreePCB(PCB *pcb);
void InsertPCB();
char * RemovePCB(PCB *pcb);
PCB * SetupPCB(char *processName, int class, int priority);

#endif