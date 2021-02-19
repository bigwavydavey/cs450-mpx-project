#include "mpx_supt.h"
//#include (Will probably need file with PCB struct)

struct queue
{
  int count;
  pcb *head;
  pcb *tail;
}
