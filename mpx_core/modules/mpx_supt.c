/*************************************************************
*	This C file contains the MPX support functions 
*	which will be used through out the semester, many set
*	flags or methods that will allow us to modify
*	The behavior of MPX as it progresses throughout 
* 	the semester.
**************************************************************/
#include "mpx_supt.h"
#include <mem/heap.h>
#include <string.h>
#include <core/serial.h>

// global variable containing parameter used when making 
// system calls via sys_req
param params;   

// global for the current module
int current_module = -1;  
static int io_module_active = 0;
static int mem_module_active = 0;


// If a student created heap manager is implemented this
// is a pointer to the student's "malloc" operation.
u32int (*student_malloc)(u32int);

// if a student created heap manager is implemented this
// is a pointer to the student's "free" operation.
int (*student_free)(void *);



/* *********************************************
*	This function is use to issue system requests
*	for service.  
*
*	Parameters:  op_code:  Requested Operation, one of
*					READ, WRITE, IDLE, EXIT
*			  device_id:  For READ & WRITE this is the
*					  device to which the request is 
*					  sent.  One of DEFAULT_DEVICE or
*					   COM_PORT
*			   buffer_ptr:  pointer to a character buffer
*					to be used with READ & WRITE request
*			   count_ptr:  pointer to an integer variable
*					 containing the number of characters
*					 to be read or written
*
*************************************************/
int sys_req( 	int  op_code,
			int device_id,
			char *buffer_ptr,
			int *count_ptr )

{
	int return_code =0;

  if (op_code == IDLE || op_code == EXIT){
    // store the process's operation request
    // triger interrupt 60h to invoke
    params.op_code = op_code;
  	asm volatile ("int $60");
  }// idle or exit

  else if (op_code == READ || op_code == WRITE) {
    // validate buffer pointer and count pointer
    if (buffer_ptr == NULL)
      return_code = INVALID_BUFFER;
    else if (count_ptr == NULL || *count_ptr <= 0)
      return_code = INVALID_COUNT;

    // if parameters are valid store in the params structure
    if ( return_code == 0){ 
      params.op_code = op_code;
      params.device_id = device_id;
      params.buffer_ptr = buffer_ptr;
      params.count_ptr = count_ptr;

      if (!io_module_active){
        // if default device
        if (op_code == READ)
          return_code = *(polling(buffer_ptr, count_ptr));
		  			
        else //must be WRITE
          return_code = serial_print(buffer_ptr);	
	    
      } else {// I/O module is implemented
        asm volatile ("int $60");
      } // NOT IO_MODULE
    }
  } else return_code = INVALID_OPERATION;
  
  return return_code;
}// end of sys_req

/*
  Procedure..: mpx_init
  Description..: Initialize MPX support software, based
			on the current module.  The operation of 
			MPX will changed based on the module selected.
			THIS must be called as the first executable 
			statement inside your command handler.

  Params..: int cur_mod (symbolic constants MODULE_R1, MODULE_R2, 			etc.  These constants can be found inside
			mpx_supt.h
*/
void mpx_init(int cur_mod)
{
  
  current_module = cur_mod;
  if (cur_mod == MEM_MODULE)
		mem_module_active = TRUE;

  if (cur_mod == IO_MODULE)
		io_module_active = TRUE;
}

/*
  Procedure..: sys_set_malloc
  Description..: Sets the memory allocation function for sys_alloc_mem
  Params..: Function pointer
*/
void sys_set_malloc(u32int (*func)(u32int))
{
  student_malloc = func;
}

/*
  Procedure..: sys_set_free
  Description..: Sets the memory free function for sys_free_mem
  Params..: s1-destination, s2-source
*/
void sys_set_free(int (*func)(void *))
{
  student_free = func;
}

/*
  Procedure..: cmd_handler
  Description..: This function has a loop to continuously handle specific
  user commands. As commands increase in quantity and complexity this
  function will eventually call a host of other functions to handle
  tasks.
  Params..: none
*/

void cmd_handler()
{
  char cmd_buffer[100];
  int buffer_size;
  int quit = 0;

  char startup_msg[100] = "\nWelcome to OS Allstars' MPX. Enter help for a list of commands.\n";
  sys_req(WRITE, DEFAULT_DEVICE, startup_msg, &buffer_size);
 
  
  while(!quit)
  {
    //Fill cmd_buffer
    sys_req(WRITE, DEFAULT_DEVICE, "\n>>", &buffer_size);
    memset(cmd_buffer, '\0', 100);
    buffer_size = 99; //reset size before each call to read
    sys_req(READ, DEFAULT_DEVICE, cmd_buffer, &buffer_size);

      
    //Version command
    if (strcmp(cmd_buffer, "version\r") == 0) // see if buffer matches version command
    {
      char current_version[60] = "\nOS Allstars' MPX Version 1.0, last updated Jan 29, 2021\n";
      sys_req(WRITE, DEFAULT_DEVICE, current_version, &buffer_size);
    }
    //Quit command
    else if (strcmp(cmd_buffer, "shutdown\r") == 0)
    {
      quit = 1; // exit cmd_handler
    }
    else if (strcmp(cmd_buffer, "help\r") == 0)
    {
      char help_msg[500] = "\nhelp: prints list of commands and explains their functionality\n";
      strcat(help_msg, "version: prints the current version of OS Allstars' MPX and most recent release date\n");
      strcat(help_msg, "shutdown: shutsdown the MPX system\n");
      strcat(help_msg, "getdate: prints current date as stored in MPX register\n");
      strcat(help_msg, "setdate --[date]: sets a user input date to the register\n");
      strcat(help_msg, "gettime: prints the current time of day as stored in MPX register\n");
      strcat(help_msg, "settime -- [time]: sets a user input time of day to the register\n");
      sys_req(WRITE, DEFAULT_DEVICE, help_msg, &buffer_size);
    }
    //else if (...) <-- other commands will look similar for help, dateandtime, etc.

    //Command not recognized
    else
    {
      char cmd_err_msg[100] = "\rInvalid input: ";
      strcat(cmd_err_msg, cmd_buffer);
      sys_req(WRITE, DEFAULT_DEVICE, cmd_err_msg, &buffer_size);
      sys_req(WRITE, DEFAULT_DEVICE, "MPX only recognizes certain commands.\nEnter help for list of commands or shutdown to exit MPX\n", &buffer_size);
    }
  }
}

/*
  Procedure..: sys_alloc_mem
  Description..: Allocates a block of memory (similar to malloc)
  Params..: Number of bytes to allocate
*/
void *sys_alloc_mem(u32int size)
{
  if (!mem_module_active)
    return (void *) kmalloc(size);
  else
    return (void *) (*student_malloc)(size);
}


/*
  Procedure..: sys_free_mem
  Description..: Frees memory
  Params..: Pointer to block of memory to free
*/
int sys_free_mem(void *ptr)
{
  if (mem_module_active)
    return (*student_free)(ptr);
  // otherwise we don't free anything
  return -1;
}

/*
  Procedure..: idle
  Description..: The idle process, used in dispatching
			it will only be dispatched if NO other
			processes are available to execute.
  Params..: None
*/
void idle()
{
  char msg[30];
  int count=0;
	
	memset( msg, '\0', sizeof(msg));
	strcpy(msg, "IDLE PROCESS EXECUTING.\n");
	count = strlen(msg);
  
  while(1){
	sys_req( WRITE, DEFAULT_DEVICE, msg, &count);
    sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
  }
}
