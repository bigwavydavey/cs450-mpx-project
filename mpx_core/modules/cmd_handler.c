#include <string.h>
#include <core/serial.h>
#include <core/io.h>
#include "mpx_supt.h"
#include "cmd_handler.h"
#include "pcb_temp_commands.h"
#include "pcb_user_commands.h"
#include "userR3Commands.h"
#include "internal_procedures.h"
#include "structs.h"
#include "R4processes.h"

int buffer_size = 99;
/**
* @brief This function is used to set the processor RTC's current time
*
* @param date_buffer: Full string representation of the time taken, unparsed or changed
* @param date_buffer_size: Size of the input string
*/
void settime(char* time_buffer, int time_buffer_size)
{
  int hr, min, sec;

  sys_req(READ, DEFAULT_DEVICE, time_buffer, &time_buffer_size);

  if (strlen(time_buffer) > 10)
    sys_req(WRITE, DEFAULT_DEVICE, time_buffer, &time_buffer_size);
    //sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid format. Aborting settime command...\n", &time_buffer_size);
  else
  {
    char * hr_s = strtok(time_buffer, ":");
    char * min_s = strtok(NULL, ":");
    char * sec_s_init = strtok(NULL, ":");
    char sec_s[3] = {sec_s_init[0], sec_s_init[1], '\0'};

    hr = atoi(hr_s);
    min = atoi(min_s);
    sec = atoi(sec_s);

    unsigned int hr_BCD = (((hr / 10) << 4) | (hr % 10));
    unsigned int min_BCD = (((min / 10) << 4) | (min % 10));
    unsigned int sec_BCD = (((sec / 10) << 4) | (sec % 10));

    if( (hr < 0) | (hr > 23) | (min < 0) | (min > 59) | (sec < 0) | (sec > 59))
      sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid numerical input. Aborting settime command...\n", &time_buffer_size);
    else
    {
      cli();
      outb(0x70, 0x04);
      outb(0x71, hr_BCD);
      outb(0x70, 0x02);
      outb(0x71, min_BCD);
      outb(0x70, 0x00);
      outb(0x71, sec_BCD);
      sti();

      sys_req(WRITE, DEFAULT_DEVICE, "\nNew time successfully set to register.\n", &time_buffer_size);
    }
  }
}

/**
* @brief This function is used to get the processor RTC's current time and print
* it to the window
*
* @param None
*
* @result None
*/
void gettime()
{
  int hr_BCD, min_BCD, sec_BCD;
  int buffer = 20;
  char time[9] = "\0";

  outb(0x70, 0x04);
  hr_BCD = inb(0x71);
  outb(0x70, 0x02);
  min_BCD = inb(0x71);
  outb(0x70, 0x00);
  sec_BCD = inb(0x71);

  int hr = (((hr_BCD >> 4) * 10) + (hr_BCD & 0xF));
  char hr_s[3];
  itoa(hr, hr_s, 10);
  int min = (((min_BCD >> 4) * 10) + (min_BCD & 0xF));
  char min_s[3];
  itoa(min, min_s, 10);
  int sec = (((sec_BCD >> 4) * 10) + (sec_BCD & 0xF));
  char sec_s[3];
  itoa(sec, sec_s, 10);

  strcat(time, hr_s);
  strcat(time, ":");
  strcat(time, min_s);
  strcat(time, ":");
  strcat(time, sec_s);

  sys_req(WRITE, DEFAULT_DEVICE, "\nThe time is: ", &buffer);
  sys_req(WRITE, DEFAULT_DEVICE, time, &buffer);
}

/**
* @brief This function is used to set the processor
         RTC's current date
*
* @param date_buffer: Full string representation of the date taken, unparsed or changed
* @param date_buffer_size: Size of the input string
*/
void setdate(char* date_buffer, int date_buffer_size)
{
  int year_full, year_dec, year_mil, month, day;

  sys_req(READ, DEFAULT_DEVICE, date_buffer, &date_buffer_size);

  if (strlen(date_buffer) > 12)
    sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid format. Aborting setdate command...\n", &date_buffer_size);
  else
  {
    char * month_s = strtok(date_buffer, "/");
    char * day_s = strtok(NULL, "/");
    char * year_s_init = strtok(NULL, "/");
    char year_mil_s[3] = {year_s_init[0], year_s_init[1], '\0'};
    char year_dec_s[3] = {year_s_init[2], year_s_init[3], '\0'};
    char year_full_s[5] = {year_s_init[0], year_s_init[1], year_s_init[2], year_s_init[3], '\0'};

    month = atoi(month_s);
    day = atoi(day_s);
    year_mil = atoi(year_mil_s);
    year_dec = atoi(year_dec_s);
    year_full = atoi(year_full_s);

    unsigned int month_BCD = (((month / 10) << 4) | (month % 10));
    unsigned int day_BCD = (((day / 10) << 4) | (day % 10));
    unsigned int year_mil_BCD = (((year_mil / 10) << 4) | (year_mil % 10));
    unsigned int year_dec_BCD = (((year_dec / 10) << 4) | (year_dec % 10));

    if( (month <= 0) | (month > 12) | (day <= 0) | (day > 31) )
      sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid date. Aborting setdate command...\n", &date_buffer_size);
    else if( (month == 2) && (day > 29) )
      sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid date. Aborting setdate command...\n", &date_buffer_size);
    else if( (month == 2) && (day == 29) )
    {
      if( ((year_full % 4 == 0) && (year_full % 100 != 0)) || (year_full % 400 == 0) )
      {
        cli();
        outb(0x70, 0x09);
        outb(0x71, year_dec_BCD);
        outb(0x70, 0x32);
        outb(0x71, year_mil_BCD);
        outb(0x70, 0x08);
        outb(0x71, month_BCD);
        outb(0x70, 0x07);
        outb(0x71, day_BCD);
        sti();

        sys_req(WRITE, DEFAULT_DEVICE, "\nNew date successfully set to register.\n", &date_buffer_size);
      }
      else
        sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid date. Aborting setdate command...\n", &date_buffer_size);
    }
    else if( ((month == 4) || (month == 6) || (month == 9) || (month == 11)) && (day ==31) )
      sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid date. Aborting setdate command...\n", &date_buffer_size);
    else
    {
      cli();
      outb(0x70, 0x09);
      outb(0x71, year_dec_BCD);
      outb(0x70, 0x32);
      outb(0x71, year_mil_BCD);
      outb(0x70, 0x08);
      outb(0x71, month_BCD);
      outb(0x70, 0x07);
      outb(0x71, day_BCD);
      sti();

      sys_req(WRITE, DEFAULT_DEVICE, "\nNew date successfully set to register.\n", &date_buffer_size);
    }
  }
}

/**
* @brief This function is used to get the processor RTC's current date and print
*        it to the window
*
* @param None
*
* @result None
*/
void getdate()
{
  int month_BCD, day_BCD, year_mil_BCD, year_dec_BCD;
  int buffer = 12;
  char date[12] = "\0";

  outb(0x70, 0x08);
  month_BCD = inb(0x71);
  outb(0x70, 0x07);
  day_BCD = inb(0x71);
  outb(0x70, 0x32);
  year_mil_BCD = inb(0x71);
  outb(0x70, 0x09);
  year_dec_BCD = inb(0x71);

  int month = (((month_BCD >> 4) * 10) + (month_BCD & 0xF));
  char month_s[3];
  itoa(month, month_s, 10);
  int day = (((day_BCD >> 4) * 10) + (day_BCD & 0xF));
  char day_s[3];
  itoa(day, day_s, 10);
  int year_mil = (((year_mil_BCD >> 4) * 10) + (year_mil_BCD & 0xF));
  char year_mil_s[3];
  itoa(year_mil, year_mil_s, 10);
  int year_dec = (((year_dec_BCD >> 4) * 10) + (year_dec_BCD & 0xF));
  char year_dec_s[3] = "\0";
  if(year_dec < 10)
  {
    char year_dec_temp[3] = "/0";
    strcat(year_dec_s, "0");
    strcat(year_dec_s, itoa(year_dec, year_dec_temp, 10));
  }
  else
    itoa(year_dec, year_dec_s, 10);

  strcat(date, month_s);
  strcat(date, "/");
  strcat(date, day_s);
  strcat(date, "/");
  strcat(date, year_mil_s);
  strcat(date, year_dec_s);

  sys_req(WRITE, DEFAULT_DEVICE, "\nThe date is: ", &buffer);
  sys_req(WRITE, DEFAULT_DEVICE, date, &buffer);
}

/**
  @brief This function is a supplementary function to
         cmd_handler() that specifically handles commands
         with user input and optional clauses.
         Splits cmd_buffer into various tokens.

  @param cmd_buffer: the buffer that is passed from cmd_buffer()
                      to this function
  @retval none
*/
void optional_cmd_handler(char * cmd_buffer)
{
  char delim[2] = " \0";
  char * cmd = strtok(cmd_buffer, delim);
  char * pcb_name = strtok(NULL, delim);
  if (strlen(pcb_name) > 10){
    sys_req(WRITE, DEFAULT_DEVICE, "\nERROR: Name must be no bigger than 10 characters", &buffer_size);
  }
  /*else if (strcmp(cmd, "createpcb") == 0)
  {
    char * class_s = strtok(NULL, delim);
    int class = atoi(class_s);
    char pcb_priority_s[10];
    char *point = pcb_priority_s;
    point = strtok(NULL, delim);
    int pcb_priority = atoi(point);

    if (pcb_priority == 10){
      serial_println("help");
    }
    CreatePCB(pcb_name, class, pcb_priority);
  }*/
  else if (strcmp(cmd, "deletepcb") == 0)
  {
    DeletePCB(pcb_name);
  }
  /*else if (strcmp(cmd, "blockpcb") == 0)
  {
    BlockPCB(pcb_name);
  }
  else if (strcmp(cmd, "unblockpcb") == 0)
  {
    UnblockPCB(pcb_name);
  }*/
  else if (strcmp(cmd, "suspendpcb") == 0)
  {
    SuspendPCB(pcb_name);
  }
  else if (strcmp(cmd, "resumepcb") == 0)
  {
    ResumePCB(pcb_name);
  }
  else if (strcmp(cmd, "setpriority") == 0)
  {
    char * pcb_priority_s = strtok(NULL, " ");
    int pcb_priority = atoi(pcb_priority_s);

    SetPCBPriority(pcb_name, pcb_priority);
  }
  else if (strcmp(cmd, "showpcb") == 0)
  {
    ShowPCB(pcb_name);
  }
  else if(strcmp(cmd_buffer, "malloc") == 0)
  {
    int bytes = atoi(pcb_name);
    AllocateMem(bytes); 
  }
  else if(strcmp(cmd_buffer, "free") == 0)
  {
    u32int address = (u32int)(atoi(pcb_name));
    FreeMem(address);  
  }
  //Command not recognized
  else
  {
    char * cmd_err_msg = "\nInvalid input: ";

    sys_req(WRITE, DEFAULT_DEVICE, cmd_err_msg, &buffer_size);
    sys_req(WRITE, DEFAULT_DEVICE, cmd_buffer, &buffer_size);
    sys_req(WRITE, DEFAULT_DEVICE, "\nMPX only recognizes certain commands.\nEnter help for list of commands or shutdown to exit MPX\n", &buffer_size);
  }
}

/**
  @brief This function provides functionality for the help
         user command.

  @param none

  @retval none
*/
void help()
{
  buffer_size = 700;

  //R1 commands
  sys_req(WRITE, DEFAULT_DEVICE, "\nhelp: prints list of commands and explains their functionality\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nversion: prints the current version of OS Allstars' MPX and most recent\nrelease date\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE,  "\nshutdown: shutsdown the MPX system. You will be asked for confirmation\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\ngetdate: prints current date as stored in MPX register\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nsetdate: sets a user input date to the register\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\ngettime: prints the current time of day as stored in MPX register\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nsettime: sets a user input time of day to the register\n", &buffer_size);

  //R2 commands
  //sys_req(WRITE, DEFAULT_DEVICE, "\ncreatepcb [pcb_name] [pcb_class] [pcb_priority]: This command will create a new PCB for the given attributes, pcb_name, pcb_priority, pcb_class.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\ndeletepcb [pcb_name]: This command will delete a selected pcb from all 4 of the PCB queues, removing them completely from the system.\n", &buffer_size);
  //sys_req(WRITE, DEFAULT_DEVICE, "\nblockpcb [pcb_name]: This command will set the selected PCB’s state to blocked and insert it into the appropriate PCB queue. pcb_name must be a valid PCB already in existence. \n", &buffer_size);
  //sys_req(WRITE, DEFAULT_DEVICE, "\nunblockpcb [pcb_name]: This command will set the selected PCB’s state to unblocked and insert it in to theappropriate PCB queue. pcb_name must be valid. \n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nsuspendpcb [pcb_name]: This command will set the selected PCB’s state to suspended and insert it in tothe appropriate PCB queue.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nresumepcb [pcb_name]: This command will set the selected PCB’s state to unsuspended and insert it in tothe appropriate PCB queue.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nsetpriority [pcb_name]: This command will set the selected PCB’s priority to a new user desired priority,\npossibly changing its location withing the queues.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nshowpcb [pcb_name]: This command will display the attributes of a selected PCB in the terminal.\n\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nsuspendpcb [pcb_name]: This command will set the selected PCB’s state to suspended and insert it in to the appropriate PCB queue.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nresumepcb [pcb_name]: This command will set the selected PCB’s state to unsuspended and insert it in to the appropriate PCB queue.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nsetpriority [pcb_name]: This command will set the selected PCB’s priority to a new user desired priority,\npossibly changing its location withing the queues.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nshowpcb [pcb_name]: This command will display the attributes of a selected PCB in the terminal.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nshowreadypcb: This command will display all processes contained within the two ready queues in the terminal.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nshowreadypcb: This command will display all processes contained within the two ready queues in the terminal.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nshowblkpcb: This command will display all processes contained within the two blocked queues in the terminal.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nshowpcbs: This commands will display all existing processes, in all four queues, in the terminal.\n", &buffer_size);
  //sys_req(WRITE, DEFAULT_DEVICE, "\nyield: This command forces the command handler to let other processes execute.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nloadr3: This command loads 5 processes into memory to test R3.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nloadr3: Loads test processes for R3\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\ninf: Loads the infinite process and adds it to ready queue.\n", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, "\nalarm: Creates an alarm process that will display a custom message at a time you will be asked to specify.\n", &buffer_size);
}
/**

  @brief This function has a loop to continuously handle specific
  user commands. As commands increase in quantity and complexity this
  function will eventually call a host of other functions to handle
  tasks. User commands are entered in a fashion similar to Linux
  command line. For example--

  @code
    >>help
  @endcode

  would be the correct way to issue to "help command".
    Currently implemented commands:
    --help
    --version: provides user with current version of MPX
    --shutdown: begins shutdown of MPX
    --settime: sets a user entered time to MPX registers
    --gettime: prints the current time, according to MPX registers
    --setdate: sets a user entered date to MPX registers
    --getdate: prints the current time, according to MPX registers

  @param none

  @retval none
*/
void cmd_handler()
{
  char cmd_buffer[100];
  int buffer_size = 99;
  int quit = 0;

  char * startup_msg = "\nWelcome to OS Allstars' MPX. Enter help for a list of commands.\n";
  sys_req(WRITE, DEFAULT_DEVICE, startup_msg, &buffer_size);


  while(!quit)
  {
    //Fill cmd_buffer
    sys_req(WRITE, DEFAULT_DEVICE, "\n>> ", &buffer_size);
    memset(cmd_buffer, '\0', 100);
    buffer_size = 99; //reset size before each call to read
    sys_req(READ, DEFAULT_DEVICE, cmd_buffer, &buffer_size);


    //Version command
    if (strcmp(cmd_buffer, "version") == 0) // see if buffer matches version command
    {
      char * current_version = "\nOS Allstars' MPX Version 4.0, last updated March 18, 2021\n";
      sys_req(WRITE, DEFAULT_DEVICE, current_version, &buffer_size);
    }
    //Shutdown command
    else if (strcmp(cmd_buffer, "shutdown") == 0)
    {
      sys_req(WRITE, DEFAULT_DEVICE, "\nAre you sure you want to shutdown the system? (y/n)\n\n>>", &buffer_size);

      char shutdown_buffer[10];
      memset(shutdown_buffer, '\0', 10);
      int shutdown_buffer_size = 9;
      sys_req(READ, DEFAULT_DEVICE, shutdown_buffer, &shutdown_buffer_size);

      if (strcmp(shutdown_buffer, "y") == 0)
      {
        //quit = 1; //exits cmd_handler
        RemovePCB(FindPCB("idle"));
        RemovePCB(FindPCB("alarm"));
        if (FindPCB("infinite") != NULL)
          RemovePCB(FindPCB("infinite"));
        sys_req(EXIT, DEFAULT_DEVICE, NULL, NULL);
      }
      else if (strcmp(shutdown_buffer, "n") == 0)
      {
        sys_req(WRITE, DEFAULT_DEVICE, "\nAborting shutdown...\n", &buffer_size);
      }
      else
        sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid input. Aborting shutdown...\n", &buffer_size);
    }

    //Help command
    else if (strcmp(cmd_buffer, "help") == 0)
    {
      help();
    }
    else if (strcmp(cmd_buffer, "settime") == 0)
    {
      sys_req(WRITE, DEFAULT_DEVICE, "\nPlease enter time in HH:MM:SS format\n", &buffer_size);

      char time_buffer[10];
      memset(time_buffer, '\0', 10);
      int time_buffer_size = 9;

      settime(time_buffer, time_buffer_size);
    }

    else if (strcmp(cmd_buffer, "gettime") == 0)
    {
      gettime();
    }

    else if (strcmp(cmd_buffer, "setdate") == 0)
    {
      sys_req(WRITE, DEFAULT_DEVICE, "\nPlease enter time in MM/DD/YYYY format\n", &buffer_size);

      char date_buffer[12];
      memset(date_buffer, '\0', 12);
      int date_buffer_size = 11;

      setdate(date_buffer, date_buffer_size);
    }

    else if (strcmp(cmd_buffer, "getdate") == 0)
    {
      getdate();
    }
    else if (strcmp(cmd_buffer, "showpcbs") == 0)
    {
      ShowAll();
    }
    else if (strcmp(cmd_buffer, "showblkpcb") == 0)
    {
      ShowBlocked();
    }
    else if (strcmp(cmd_buffer, "showreadypcb") == 0)
    {
      ShowReady();
    }
    else if(strcmp(cmd_buffer, "inf") == 0)
    {
       struct pcb *inf_proc = SetupPCB("infinite", 1, 0);
       struct context *inf_context = (struct context *)(inf_proc -> top);
       memset(inf_context, 0, sizeof(struct context));
       inf_context -> fs = 0x10;
       inf_context -> gs = 0x10;
       inf_context -> ds = 0x10;
       inf_context -> es = 0x10;
       inf_context -> cs = 0x8;
       inf_context -> ebp = (u32int)(inf_proc-> stack);
       inf_context -> esp = (u32int)(inf_proc -> top);
       inf_context -> eip = (u32int)infinite_proc;
       inf_context -> eflags = 0x202;
       InsertPCB(inf_proc);
    }
    else if (strcmp(cmd_buffer, "loadr3") == 0)
      loadr3();
    else if (strcmp(cmd_buffer, "alarm") == 0)
    {
      sys_req(WRITE, DEFAULT_DEVICE, "\nPlease enter time in HH:MM:SS format\n", &buffer_size);
      int alarm_time_size = 10;
      char alarm_time[alarm_time_size];
      memset(alarm_time, '\0', alarm_time_size);
      sys_req(READ, DEFAULT_DEVICE, alarm_time, &alarm_time_size);

      sys_req(WRITE, DEFAULT_DEVICE, "\nPlease enter message\n", &buffer_size);
      int alarm_msg_size = 50;
      char alarm_msg[alarm_msg_size];
      memset(alarm_msg, '\0', alarm_msg_size);
      sys_req(READ, DEFAULT_DEVICE, alarm_msg, &alarm_msg_size);

      add_alarm( alarm_time, alarm_msg );
    }
    else if(strcmp(cmd_buffer, "initheap") == 0)
      InitializeHeap(1000);
    else if(strcmp(cmd_buffer, "isempty") == 0)
    {
      if (isEmpty() == 1)
      {
        char * empty_msg = "\nThe allocated list is empty.";
        int empty_msg_size = strlen(empty_msg);
        sys_req(WRITE, DEFAULT_DEVICE, empty_msg, &empty_msg_size);
      }
      else
      {
        char * not_empty_msg = "\nThe allocated list contains blocks.";
        int not_empty_msg_size = strlen(not_empty_msg);
        sys_req(WRITE, DEFAULT_DEVICE, not_empty_msg, &not_empty_msg_size);
      }
    }
    else if(strcmp(cmd_buffer, "showfree") == 0){
      showFree();
    }
    else if(strcmp(cmd_buffer, "showmalloc") == 0)
      showAllocated();
    //Command not recognized
    else
    {
      optional_cmd_handler(cmd_buffer);
    }
  sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
  }
}
