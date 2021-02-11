#include <string.h>
#include <core/serial.h>
#include <core/io.h>
#include "mpx_supt.h"
#include "cmd_handler.h"
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

  if (strlen(time_buffer) > 9)
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
      sys_req(WRITE, DEFAULT_DEVICE, "\nNew time successfully set to register.\n", &buffer_size);
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

  sys_req(WRITE, DEFAULT_DEVICE, "\nThe time is: ", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, time, &buffer_size);
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
  int /*year_full, */year_dec, year_mil, month, day;

  sys_req(READ, DEFAULT_DEVICE, date_buffer, &date_buffer_size);

  if (strlen(date_buffer) > 11)
    sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid format. Aborting setdate command...\n", &buffer_size);
  else
  {
    char * month_s = strtok(date_buffer, "/");
    char * day_s = strtok(NULL, "/");
    char * year_s_init = strtok(NULL, "/");
    char year_mil_s[3] = {year_s_init[0], year_s_init[1], '\0'};
    char year_dec_s[3] = {year_s_init[2], year_s_init[3], '\0'};
    //char year_full_s[5] = {year_s_init[0], year_s_init[1], year_s_init[2], year_s_init[3], '\0'};

    month = atoi(month_s);
    day = atoi(day_s);
    year_mil = atoi(year_mil_s);
    year_dec = atoi(year_dec_s);
    //year_full = atoi(year_full_s);

    unsigned int month_BCD = (((month / 10) << 4) | (month % 10));
    unsigned int day_BCD = (((day / 10) << 4) | (day % 10));
    unsigned int year_mil_BCD = (((year_mil / 10) << 4) | (year_mil % 10));
    unsigned int year_dec_BCD = (((year_dec / 10) << 4) | (year_dec % 10));

    if( (month <= 0) | (month > 12) | (day <= 0) | (day > 31) )
      sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid numerical input. Aborting setdate command...\n", &buffer_size);
    else if( (month == 2) && (day > 29) )
      sys_req(WRITE, DEFAULT_DEVICE, "\nInvalid numerical input. Aborting setdate command...\n", &buffer_size);
    /*else if( (strcmp(month_s, "02") == 0) && (strcmp(day_s, "29") == 0))
    {
      serial_print("Feb 29th");
      if(year_full % 400 == 0)
      {
        if(year_full % 100 != 0)
        {
          if(year_full % 4 == 0)
          {
            serial_print("leap_year");
          }
        }
      }
        //sys_rep(WRITE, DEFAULT_DEVICE, "\nInvalid date, this year is not a leap year. Date set to 3/1 of the specified year.\n", &buffer_size);
    }*/
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
      sys_req(WRITE, DEFAULT_DEVICE, "\nNew date successfully set to register.\n", &buffer_size);
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
  char date[11] = "\0";

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

  sys_req(WRITE, DEFAULT_DEVICE, "\nThe date is: ", &buffer_size);
  sys_req(WRITE, DEFAULT_DEVICE, date, &buffer_size);
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
    if (strcmp(cmd_buffer, "version\r") == 0) // see if buffer matches version command
    {
      char * current_version = "\nOS Allstars' MPX Version 1.0, last updated Feb 11, 2021\n";
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
      char * help_msg = "\nhelp: prints list of commands and explains their functionality\n";

      strcat(help_msg, "\nversion: prints the current version of OS Allstars' MPX and most recent release date\n");
      strcat(help_msg, "\nshutdown: shutsdown the MPX system> You will be asked for confirmation\n");
      strcat(help_msg, "\ngetdate: prints current date as stored in MPX register\n");
      strcat(help_msg, "\nsetdate: sets a user input date to the register\n");
      strcat(help_msg, "\ngettime: prints the current time of day as stored in MPX register\n");
      strcat(help_msg, "\nsettime: sets a user input time of day to the register\n");
      sys_req(WRITE, DEFAULT_DEVICE, help_msg, &buffer_size);
    }

    else if (strcmp(cmd_buffer, "settime\r") == 0)
    {
      sys_req(WRITE, DEFAULT_DEVICE, "\nPlease enter time in HH:MM:SS format\n", &buffer_size);

      char time_buffer[9];
      memset(time_buffer, '\0', 9);
      int time_buffer_size = 8;

      settime(time_buffer, time_buffer_size);
    }

    else if (strcmp(cmd_buffer, "gettime\r") == 0)
    {
      gettime();
    }

    else if (strcmp(cmd_buffer, "setdate\r") == 0)
    {
      sys_req(WRITE, DEFAULT_DEVICE, "Please enter time in MM/DD/YYYY format\n", &buffer_size);

      char date_buffer[11];
      memset(date_buffer, '\0', 11);
      int date_buffer_size = 10;

      setdate(date_buffer, date_buffer_size);
    }

    else if (strcmp(cmd_buffer, "getdate\r") == 0)
    {
      getdate();
    }

    //Command not recognized
    else
    {
      char * cmd_err_msg = "\nInvalid command: ";
      sys_req(WRITE, DEFAULT_DEVICE, cmd_err_msg, &buffer_size);
      sys_req(WRITE, DEFAULT_DEVICE, cmd_buffer, &buffer_size);
      sys_req(WRITE, DEFAULT_DEVICE, "\nMPX only recognizes certain commands.\nEnter help for list of commands or shutdown to exit MPX\n", &buffer_size);
    }
  }
}
