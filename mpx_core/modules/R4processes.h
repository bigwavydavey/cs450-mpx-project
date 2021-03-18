#include "structs.h"
#include "userR3Commands.h"
#include "procsr3.h"
#include "internal_procedures.h"
#include "mpx_supt.h"

#include <string.h>
#include <core/serial.h>
#include <core/io.h>

struct alarm_list alarms;

void infinite_proc();

void add_alarm( char* alarm_time, char* alarm_msg );

void alarm_proc();
