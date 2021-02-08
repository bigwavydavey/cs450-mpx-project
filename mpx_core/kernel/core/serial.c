/*
  ----- serial.c -----

  Description..: Contains methods and variables used for
    serial input and output.
*/

#include <stdint.h>
#include <string.h>

#include <core/io.h>
#include <core/serial.h>
#include <modules/mpx_supt.h>

#define NO_ERROR 0

// Active devices used for serial I/O
int serial_port_out = 0;
int serial_port_in = 0;

/*
  Procedure..: init_serial
  Description..: Initializes devices for user interaction, logging, ...
*/
int init_serial(int device)
{
  outb(device + 1, 0x00); //disable interrupts
  outb(device + 3, 0x80); //set line control register
  outb(device + 0, 115200/9600); //set bsd least sig bit
  outb(device + 1, 0x00); //brd most significant bit
  outb(device + 3, 0x03); //lock divisor; 8bits, no parity, one stop
  outb(device + 2, 0xC7); //enable fifo, clear, 14byte threshold
  outb(device + 4, 0x0B); //enable interrupts, rts/dsr set
  (void)inb(device);      //read bit to reset port
  return NO_ERROR;
}

/*
  Procedure..: serial_println
  Description..: Writes a message to the active serial output device.
    Appends a newline character.
*/
int serial_println(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  outb(serial_port_out,'\r');
  outb(serial_port_out,'\n');  
  return NO_ERROR;
}

/*
  Procedure..: serial_print
  Description..: Writes a message to the active serial output device.
*/
int serial_print(const char *msg)
{
  int i;
  for(i=0; *(i+msg)!='\0'; i++){
    outb(serial_port_out,*(i+msg));
  }
  if (*msg == '\r') outb(serial_port_out,'\n');
  return NO_ERROR;
}

/*int (*student_read)(char *buffer, int *count);
  Procedure..: set_serial_out
  Description..: Sets serial_port_out to the given device address.
    All serial output, such as that from serial_println, will be
    directed to this device.
*/
int set_serial_out(int device)
{
  serial_port_out = device;
  return NO_ERROR;
}

/*
  Procedure..: set_serial_in
  Description..: Sets serial_port_in to the given device address.
    All serial input, such as console input via a virtual machine,
    QEMU/Bochs/etc, will be directed to this device.
*/
int set_serial_in(int device)
{
  serial_port_in = device;
  return NO_ERROR;
}

int *polling(char *buffer, int *count){
  int cursor_pos=0;
  int num_characters = 0;
  int loop_control = 0;
  //start of polling loop
  while (1){
    if (inb(COM1 + 5) & 1){
      char letter = inb(COM1);
      if (letter == 13){
        buffer[num_characters] = letter;
        serial_print(buffer + num_characters);
        break;
      }
      else if (letter == 127){
        if (cursor_pos > 0){
          if (cursor_pos == num_characters){
            buffer[cursor_pos] = '\0';
            cursor_pos--;
            num_characters--;
            serial_print("\x1b[1D");
            serial_print("\x1b[0K");
          }
          else{
            num_characters--;
            cursor_pos--;
            loop_control = cursor_pos;
            while (loop_control < num_characters){
              buffer[loop_control] = buffer[loop_control + 1];
              loop_control++;
            }
            buffer[num_characters] = '\0';
            serial_print("\x1b[0K");
            serial_print("\x1b[1D");
            serial_print("\x1b[s");
            serial_print(buffer + cursor_pos);
            serial_print("\x1b[u");
          }
        }
      }
      else if (letter == 126){
        if (cursor_pos != num_characters){
          loop_control = cursor_pos + 1;
          num_characters--;
          while (loop_control < num_characters){
            buffer[loop_control] = buffer[loop_control + 1];
            loop_control++;
          }
          buffer[num_characters] = '\0';
          serial_print("\x1b[0K");
          serial_print("\x1b[s");
          serial_print(buffer + cursor_pos);
          serial_print("\x1b[u");
        }
      }
      else if (letter == 27){
        letter = inb(COM1);
        letter = inb(COM1);
        if (letter == 68){
          if (cursor_pos != 0){
            serial_print("\x1b[1D");
            cursor_pos--;
          }
        }
        else if (letter == 67){
          if (cursor_pos != num_characters){
            serial_print("\x1b[1C");
            cursor_pos++;
          }
        }
        else if (letter == 66){

        }
        else if (letter == 65){

        }
      }
      else{
        if (cursor_pos == num_characters){
          buffer[num_characters] = letter;
          serial_print(buffer + num_characters);
        }
        else {
          loop_control = num_characters;
          while (loop_control > cursor_pos) {
            buffer[loop_control] = buffer[loop_control -1];
            loop_control--;
          }
          buffer[cursor_pos] = letter;
          serial_print("\x1b[0K");
          serial_print("\x1b[s");
          serial_print(buffer + cursor_pos);
          serial_print("\x1b[u");
          serial_print("\x1b[1C");
        }
        cursor_pos++;
        num_characters++;
      }
    }
  }
// remove the following line after implementing your module, this is present
// just to allow the program to compile before R1 is complete

return count;
}

