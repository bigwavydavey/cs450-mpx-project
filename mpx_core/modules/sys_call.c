#include "mpx_supt.h"
#include "structs.h"
#include "internal_procedures.h"
#include "string.h"
#include "serial_port_driver.h"
#include <core/serial.h>

struct pcb* cop;
struct context* reference;
struct io_queue* io_queue;
//struct iocb* new_iocb = sys_alloc_mem(sizeof(struct iocb));

/**
* @brief Prepares the system for the next ready process to begin/resume execution
*
* @param registers: A indirect memory operand pointing to the top of the stack
*
* @retval u32int*: Returns a new stack pointer
*/
u32int* sys_call(struct context *registers )
{
  //int com_ercode;
  //struct iocb* new_iocb = sys_alloc_mem(sizeof(struct iocb));

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
    /*else if( params.op_code == READ || params.op_code == WRITE ){
        cop->state = 2;
        InsertPCB(cop);
        new_iocb->process = cop;
        new_iocb->buffer = params.buffer_ptr;
        new_iocb->buffer_size = params.count_ptr;
        new_iocb->device = device;
        new_iocb->operation = params.op_code;

        if(io_queue->head == NULL)
        {
          io_queue->head = new_iocb;
          io_queue->tail = new_iocb;
        }
        else
        {
          struct iocb* iocb_ptr = io_queue->head;
          while( iocb_ptr->next != NULL )
            iocb_ptr = iocb_ptr->next;

          iocb_ptr->next = new_iocb;
          iocb_ptr->next->prev = iocb_ptr;
          io_queue->tail = new_iocb;
          io_queue->count = io_queue->count + 1;
        }
        if(io_queue->head->next == NULL)
        {
          if(params.op_code == READ)
          {
            com_ercode = com_read(new_iocb->buffer, new_iocb->buffer_size);
            cop->state = 0;
            InsertPCB(cop);*/
          /*}
          else if(params.op_code == WRITE)
          {
            klogv("first time");
            com_ercode = com_write(new_iocb->buffer, new_iocb->buffer_size);
            cop->state = 0;
            InsertPCB(cop);*/
          /*}
        }
    }
  }*/

  /*if(com_ercode < 0){
    int buff = 10;
    sys_req(WRITE, DEFAULT_DEVICE, "com error", &buff);
  }
  int check_flag = 1;
  int * check_flag_ptr = &check_flag;
  if(new_iocb->device->event_flag == check_flag_ptr)
  {
    klogv("check");
    struct iocb* iocb_ptr = io_queue->head;
    io_queue->head = io_queue->head->next;
    iocb_ptr->next = NULL;
    iocb_ptr->prev = NULL;
    io_queue->count = io_queue->count - 1;

    iocb_ptr->process->state = 0; //ready_not_suspended, not sure if should be ready_suspended

    if(io_queue->head != NULL)
    {
      if(new_iocb->operation == READ)
      {
        com_ercode = com_read(new_iocb->buffer, new_iocb->buffer_size);
        cop->state = 0;
        InsertPCB(cop);
      }
      else if(new_iocb->operation == WRITE)
      {
        klogv("second");
        com_ercode = com_write(new_iocb->buffer, new_iocb->buffer_size);
        cop->state = 0;
        InsertPCB(cop);
      }
    }*/
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
