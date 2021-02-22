#ifndef _INTERNAL_PROCEDURES_H
#define _INTERNAL_PROCEDURES_H

struct pcb * AllocatePCB();
struct pcb * FindPCB(char *processName);
void FreePCB(struct pcb *PCB);
void InsertPCB();
void RemovePCB(struct pcb *PCB);
struct pcb * SetupPCB(char *processName, int class, int priority);

#endif