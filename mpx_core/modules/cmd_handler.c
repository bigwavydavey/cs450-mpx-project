#include <string.h>
#include <core/serial.h>
#include <core/io.h>

void cmd_handler()
{
  char cmd_buffer[100];
  int buffer_size;
  int quit = 0;

  char * startup_msg = "\nWelcome to OS Allstars' MPX. Enter help for a list of commands.\n";
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
    //Shutdown command
    else if (strcmp(cmd_buffer, "shutdown\r") == 0)
    {
      sys_req(WRITE, DEFAULT_DEVICE, "\nAre you sure you want to shutdown the system? (y/n)\n\n>>", &buffer_size);

      char shutdown_buffer[10];
      memset(shutdown_buffer, '\0', 10);
      int shutdown_buffer_size = 9;
      sys_req(READ, DEFAULT_DEVICE, shutdown_buffer, &shutdown_buffer_size);

      if (strcmp(shutdown_buffer, "y\r") == 0)
        quit = 1; //exits cmd_handler
      else if (strcmp(shutdown_buffer, "n\r") == 0)
      {
        sys_req(WRITE, DEFAULT_DEVICE, "\nAborting shutdown...\n", &buffer_size);
      }
      else
        sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid input. Aborting shutdown...\n", &buffer_size);
    }

    //Help command
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

    //settime command
    
    else if (strcmp(cmd_buffer, "settime\r") == 0)
    {
      int hr, min, sec;

      sys_req(WRITE, DEFAULT_DEVICE, "Please enter time in HH:MM:SS format\n", &buffer_size);

      char time_buffer[9];
      memset(time_buffer, '\0', 9);
      int time_buffer_size = 8;
      sys_req(READ, DEFAULT_DEVICE, time_buffer, &time_buffer_size);

      if (strlen(time_buffer) > 11)
        sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid format. Aborting settime command...\n", &buffer_size);
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
          sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid numerical input. Aborting settime command...\n", &buffer_size);
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
        }

        sys_req(WRITE, DEFAULT_DEVICE, "\nTime succesfully updated\n", &buffer_size);
      }
    }

    else if (strcmp(cmd_buffer, "gettime\r") == 0)
    {
      int hr_BCD, min_BCD, sec_BCD;
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

      sys_req(WRITE, DEFAULT_DEVICE, time, &buffer_size);

    }
    
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