#include "mpx_supt.h"
#include "structs.h"
#include "internal_procedures.h"

struct pcb* cop;
struct context *reference;

u32int* sys_call(struct context *registers )
{
  if( cop == NULL )
  {
    reference = registers;
  }
  else
  {
    extern param params;
    if( params.op_code == IDLE )
    {
      cop->top = (unsigned char*)registers;
      InsertPCB(cop);
    }
    else if( params.op_code == EXIT ){
      FreePCB(cop);
    }
  }

  if( ready_not_suspended.head != NULL )
  {
    struct pcb* rdy_process = ready_not_suspended.head;
    RemovePCB(rdy_process);
    cop = rdy_process;
    return (u32int*)cop->top;
  }
  else
    return (u32int*)reference;
}
