#include "mpx_supt.h"
#include "structs.h"
#include "internal_procedures.h"

struct pcb* cop;
struct context *reference;

/**
* @brief Prepares the system for the next ready process to begin/resume execution
*
* @param registers: A indirect memory operand pointing to the top of the stack
*
* @retval u32int*: Returns a new stack pointer
*/
u32int* sys_call(struct context *registers )
{
  struct pcb* temp = NULL;
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
      cop->state = 0;
      temp = cop;
      //InsertPCB(cop);
    }
    else if( params.op_code == EXIT ){
      FreePCB(cop);
    }
  }

  if( ready_not_suspended.head != NULL )
  {
    struct pcb* rdy_process = ready_not_suspended.head;
    RemovePCB(rdy_process);
    rdy_process->state = 5;
    if(temp != NULL)
      InsertPCB(temp);
    cop = rdy_process;
    return (u32int*)cop->top;
  }
  else
    return (u32int*)reference;
}
