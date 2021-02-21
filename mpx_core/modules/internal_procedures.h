#ifndef _INTERNAL_PROCEDURES_H
#define _INTERNAL_PROCEDURES_H

pcb * AllocatePCB();
pcb * FindPCB(char *processName);
void FreePCB(pcb *PCB);
void InsertPCB();
void RemovePCB(pcb *PCB);
pcb * SetupPCB(char *processName, int class, int priority);

#endif