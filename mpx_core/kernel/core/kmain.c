/*
  ----- kmain.c -----

  Description..: Kernel main. The first function called after
      the bootloader. Initialization of hardware, system
      structures, devices, and initial processes happens here.

      Initial Kernel -- by Forrest Desjardin, 2013,
      Modifications by:    Andrew Duncan 2014,  John Jacko 2017
      				Ben Smith 2018, and Alex Wilson 2019
*/

#include <stdint.h>
#include <string.h>
#include <system.h>

#include <core/io.h>
#include <core/serial.h>
#include <core/tables.h>
#include <core/interrupts.h>
#include <mem/heap.h>
#include <mem/paging.h>

#include "modules/mpx_supt.h"
#include "modules/cmd_handler.h"
#include "modules/structs.h"
#include "modules/internal_procedures.h"
#include "modules/pcb_user_commands.h"
#include "modules/R4processes.h"



void kmain(void)
{
   extern uint32_t magic;
   // Uncomment if you want to access the multiboot header
   // extern void *mbd;
   // char *boot_loader_name = (char*)((long*)mbd)[16];


   // 0) Initialize Serial I/O
   // functions to initialize serial I/O can be found in serial.c
   // there are 3 functions to call
   init_serial(COM1);
   set_serial_in(COM1);
   set_serial_out(COM1);

   klogv("Starting MPX boot sequence...");
   klogv("Initialized serial I/O on COM1 device...");

   // 1) Initialize the support software by identifying the current
   //     MPX Module.  This will change with each module.
   // you will need to call mpx_init from the mpx_supt.c
   mpx_init(MODULE_R2);

   // 2) Check that the boot was successful and correct when using grub
   // Comment this when booting the kernel directly using QEMU, etc.
   if ( magic != 0x2BADB002 ){
     //kpanic("Boot was not error free. Halting.");
   }

   // 3) Descriptor Tables -- tables.c
   //  you will need to initialize the global
   // this keeps track of allocated segments and pages
   init_gdt();
   init_idt();
   klogv("Initializing descriptor tables...");



    // 4)  Interrupt vector table --  tables.c
    // this creates and initializes a default interrupt vector table
    // this function is in tables.c
    init_pic();
    init_irq();
    klogv("Interrupt vector table initialized!");

   // 5) Virtual Memory -- paging.c  -- init_paging
   //  this function creates the kernel's heap
   //  from which memory will be allocated when the program calls
   // sys_alloc_mem UNTIL the memory management module  is completed
   // this allocates memory using discrete "pages" of physical memory
   // NOTE:  You will only have about 70000 bytes of dynamic memory
   //
   init_paging();
   klogv("Initializing virtual memory...");


   // 6) Call YOUR command handler -  interface method
   klogv("Transferring control to commhand...");
   //cmd_handler();

   struct pcb *comhand = SetupPCB("comhand", 0, 9);
   RemovePCB(comhand);
   comhand->state = 0;
   InsertPCB(comhand);
   struct context *cp_1 = (struct context *)(comhand -> top);
   memset(cp_1, 0, sizeof(struct context));
   cp_1 -> fs = 0x10;
   cp_1 -> gs = 0x10;
   cp_1 -> ds = 0x10;
   cp_1 -> es = 0x10;
   cp_1 -> cs = 0x8;
   cp_1 -> ebp = (u32int)(comhand -> stack);
   cp_1 -> esp = (u32int)(comhand -> top);
   cp_1 -> eip = (u32int)cmd_handler;
   cp_1 -> eflags = 0x202;

   struct pcb *idle_p = SetupPCB("idle", 0, 9);
   RemovePCB(idle_p);
   idle_p->state = 0;
   InsertPCB(idle_p);
   struct context *idle_context = (struct context *)(idle_p -> top);
   memset(idle_context, 0, sizeof(struct context));
   idle_context -> fs = 0x10;
   idle_context -> gs = 0x10;
   idle_context -> ds = 0x10;
   idle_context -> es = 0x10;
   idle_context -> cs = 0x8;
   idle_context -> ebp = (u32int)(idle_p -> stack);
   idle_context -> esp = (u32int)(idle_p -> top);
   idle_context -> eip = (u32int)idle;
   idle_context -> eflags = 0x202;
   asm volatile ("int $60");

   // 7) System Shutdown on return from your command handler
   klogv("Starting system shutdown procedure...");

   /* Shutdown Procedure */
   klogv("Shutdown complete. You may now turn off the machine. (QEMU: C-a x)");
   hlt();
}
