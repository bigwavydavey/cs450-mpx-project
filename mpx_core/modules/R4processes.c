#include "structs.h"
#include "userR3Commands.h"
#include "procsr3.h"
#include "internal_procedures.h"
#include "mpx_supt.h"

#include <string.h>

void infinite_proc()
{
	int quit = 1;
	char * idle_msg = "\nIdle process has been dispatched.";
	int idle_msg_size = 34;
	
	while (!quit)
	{
		sys_req(WRITE, DEFAULT_DEVICE, idle_msg, &idle_msg_size);
		sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
	}

}