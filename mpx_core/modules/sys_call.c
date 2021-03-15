#include "mpx_supt.h"
#include "structs.h"
#include "internal_procedures.h"

struct pcb* cop;
context *reference;

u32int* sys_call( context *registers )
{
  if( cop == NULL )
  {
    reference = registers;
  }
  else
  {
    if( params.op_code == IDLE )
    {
      cop->top = (unsigned char*)registers;
    }
    else if( params.op_code == EXIT )
      FreePCB(cop);
  }

  if( ready_not_suspended.head != NULL )
  {
    struct pcb* rdy_process = ready_not_suspended.head;
    RemovePCB(rdy_process);
    rdy_process->state = 5;
    cop = rdy_process;
    return cop->top;
  }
  else
    return cop->top;
}
