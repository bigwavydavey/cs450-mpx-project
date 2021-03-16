#include "structs.h"
#include "userR3Commands.h"
#include "procsr3.h"
#include "internal_procedures.h"
#include "mpx_supt.h"

#include <string.h>
#include <core/io.h>

struct alarm_list alarms;

/*void infinite_proc()
{
	int quit = 1;
	char * idle_msg = "\nIdle process has been dispatched.";
	int idle_msg_size = 34;

	while (!quit)
	{
		sys_req(WRITE, DEFAULT_DEVICE, idle_msg, &idle_msg_size);
		sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
	}

}*/

void add_alarm( char* alarm_time, char* alarm_msg )
{
  struct alarm *al;
  al = sys_alloc_mem(sizeof(struct alarm));
  strcpy(al->alarm_time, alarm_time);
  strcpy(al->alarm_msg, alarm_msg);
  if (alarms.head == NULL){
    alarms.head = al;
    alarms.tail = al;
  }
  else {
    al->prev = alarms.tail;
    alarms.tail->next = al;
    alarms.tail = al;
  }
  alarms.count++;
}

void alarm_proc()
{
  char alarm_time_copy[10];
  while (1){
    if (alarms.head != NULL){

      int input_hr, input_min, input_sec;
	    int hr_BCD, min_BCD, sec_BCD;
      int buffer = 50;

      outb(0x70, 0x04);
      hr_BCD = inb(0x71);
      outb(0x70, 0x02);
      min_BCD = inb(0x71);
      outb(0x70, 0x00);
      sec_BCD = inb(0x71);

      int hr = (((hr_BCD >> 4) * 10) + (hr_BCD & 0xF));
      int min = (((min_BCD >> 4) * 10) + (min_BCD & 0xF));
      int sec = (((sec_BCD >> 4) * 10) + (sec_BCD & 0xF));
      struct alarm *current = alarms.head;
      while (current != NULL){
        memset(alarm_time_copy, '\0', 10);
        strcpy(alarm_time_copy, current->alarm_time);
        char * hr_s = strtok(alarm_time_copy, ":");
        char * min_s = strtok(NULL, ":");
        char * sec_s_init = strtok(NULL, ":");
        char sec_s[3] = {sec_s_init[0], sec_s_init[1], '\0'};
        input_hr = atoi(hr_s);
        input_min = atoi(min_s);
        input_sec = atoi(sec_s);
	      if( hr > input_hr || ( hr == input_hr && min > input_min ) || ( hr == input_hr && min == input_min && sec >= input_sec ) )
	      {
		      sys_req(WRITE, DEFAULT_DEVICE, "\n", &buffer);
          sys_req(WRITE, DEFAULT_DEVICE, "ALARM: ", &buffer);
          sys_req(WRITE, DEFAULT_DEVICE, current->alarm_msg, &buffer);
          sys_req(WRITE, DEFAULT_DEVICE, "\n", &buffer);
          if (alarms.count == 1){
            alarms.tail = NULL;
            alarms.head = NULL;
          }
          else if (current->prev == NULL){
            alarms.head = current->next;
            alarms.head->prev = NULL;
          }
          else if (current->next == NULL){
            alarms.tail = current->prev;
            alarms.tail->next = NULL;
          }
          else{
            current->prev->next = current->next;
            current->next->prev = current->prev;
          }
          alarms.count--;
	      }

        current = current->next;
      }
    }

    sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
  }

}
