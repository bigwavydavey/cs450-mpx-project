#ifndef _INTERNAL_PROCEDURES_H
#define _INTERNAL_PROCEDURES_H

#include <system.h>
/*
extern struct queue ready_suspended;
extern struct queue ready_not_suspended;
extern struct queue blocked_suspended;
extern struct queue blocked_not_suspended;
*/

struct pcb * AllocatePCB();
struct pcb * FindPCB(char *processName);
void FreePCB(struct pcb *PCB);
void InsertPCB();
void RemovePCB(struct pcb *PCB);
struct pcb * SetupPCB(char *processName, int class, int priority);
void InitializeHeap(u32int size);
void AllocateMem(u32int address);
void FreeMem();
int isEmpty();
void showFree();
void showAllocated();

#endif
