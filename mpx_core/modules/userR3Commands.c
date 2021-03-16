#include "structs.h"
#include "userR3Commands.h"
#include "procsr3.h"
#include "internal_procedures.h"
#include <string.h>
void yield(){
	asm volatile ("int $60");
}

void loadr3(){
	struct pcb *new_pcb1 = SetupPCB("proc1", 1, 1);
	struct context *cp_1 = (struct context *)(new_pcb1 -> top);
	memset(cp_1, 0, sizeof(struct context));
	cp_1 -> fs = 0x10;
	cp_1 -> gs = 0x10;
	cp_1 -> ds = 0x10;
	cp_1 -> es = 0x10;
	cp_1 -> cs = 0x8;
	cp_1 -> ebp = (u32int)(new_pcb1 -> stack);
	cp_1 -> esp = (u32int)(new_pcb1 -> top);
	cp_1 -> eip = (u32int)proc1;
	cp_1 -> eflags = 0x202;
	struct pcb *new_pcb2 = SetupPCB("proc2", 1, 2);
	struct context *cp_2 = (struct context *)(new_pcb2 -> top);
	memset(cp_2, 0, sizeof(struct context));
	cp_2 -> fs = 0x10;
	cp_2 -> gs = 0x10;
	cp_2 -> ds = 0x10;
	cp_2 -> es = 0x10;
	cp_2 -> cs = 0x8;
	cp_2 -> ebp = (u32int)(new_pcb2 -> stack);
	cp_2 -> esp = (u32int)(new_pcb2 -> top);
	cp_2 -> eip = (u32int)proc2;
	cp_2 -> eflags = 0x202;
	struct pcb *new_pcb3 = SetupPCB("proc3", 1, 3);
	struct context *cp_3 = (struct context *)(new_pcb3 -> top);
	memset(cp_3, 0, sizeof(struct context));
	cp_3 -> fs = 0x10;
	cp_3 -> gs = 0x10;
	cp_3 -> ds = 0x10;
	cp_3 -> es = 0x10;
	cp_3 -> cs = 0x8;
	cp_3 -> ebp = (u32int)(new_pcb3 -> stack);
	cp_3 -> esp = (u32int)(new_pcb3 -> top);
	cp_3 -> eip = (u32int)proc3;
	cp_3 -> eflags = 0x202;
	struct pcb *new_pcb4 = SetupPCB("proc4", 1, 4);
	struct context *cp_4 = (struct context *)(new_pcb4 -> top);
	memset(cp_4, 0, sizeof(struct context));
	cp_4 -> fs = 0x10;
	cp_4 -> gs = 0x10;
	cp_4 -> ds = 0x10;
	cp_4 -> es = 0x10;
	cp_4 -> cs = 0x8;
	cp_4 -> ebp = (u32int)(new_pcb4 -> stack);
	cp_4 -> esp = (u32int)(new_pcb4 -> top);
	cp_4 -> eip = (u32int)proc4;
	cp_4 -> eflags = 0x202;
	struct pcb *new_pcb5 = SetupPCB("proc5", 1, 5);
	struct context *cp_5 = (struct context *)(new_pcb5 -> top);
	memset(cp_5, 0, sizeof(struct context));
	cp_5 -> fs = 0x10;
	cp_5 -> gs = 0x10;
	cp_5 -> ds = 0x10;
	cp_5 -> es = 0x10;
	cp_5 -> cs = 0x8;
	cp_5 -> ebp = (u32int)(new_pcb5 -> stack);
	cp_5 -> esp = (u32int)(new_pcb5 -> top);
	cp_5 -> eip = (u32int)proc5;
	cp_5 -> eflags = 0x202;
	asm volatile ("int $60");
}