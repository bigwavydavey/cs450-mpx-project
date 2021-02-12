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
/**
* This function is used to navigate the user interface,
* by taking in keyboard inputs, wrties them to the console
* and stores the input in a buffer 
*
* @param  beffer: the buffer is a pointer to the character array 
*                 in the command handler. The character array stores
*                 character input from the user
* @param  count: pointer to a integer size of the buffer used in sys_req

  @retval count: point to integer size of the buffer used in sys_req
* 
*/

int *polling(char *buffer, int *count){
  //instantiates position variables
  int cursor_pos = 0;
  int num_characters = 0;
  int loop_control = 0;
  //start of polling loop
  while (1){
    //if statement to check if key is pressed and stores value if so
    if (inb(COM1 + 5) & 1){
      if (num_characters == 99){
        serial_println("\nERROR: Entered in too many characters");
        break;
      }
      char letter = inb(COM1);
      // checks to see if enter key was pressed, if pressed break loop
      if (letter == 13){
        buffer[num_characters] = letter;
        serial_print(buffer + num_characters);
        break;
      }
      // checks to see if backspace key is pressed.
      else if (letter == 127){
        // checks to make sure the cursor is within the buffer
        if (cursor_pos > 0){
          // checks to see if cursor is at the end of the array. if so, assign last known character to null 
          //decrease the cursor position and number of characters and clear last written character from terminal
          if (cursor_pos == num_characters){
            buffer[cursor_pos] = '\0';
            cursor_pos--;
            num_characters--;
            serial_print("\x1b[1D");
            serial_print("\x1b[0K");
          }
          //checks to see if cursor is in the middle of the buffer. If so, decrease cursor position and number of characters
          // and move every character to the left one space until deleted character
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
      //checks to see if delete key was pressed
      else if (letter == 126){
        // checks to see if cursor positon is in the middle of the buffer. If so delete character to the right of cursor position
        // and move remaining characters left to fill deleted character
        if (cursor_pos != num_characters){
          loop_control = cursor_pos;
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
      // checks to see if any arrow keys are pressed
      else if (letter == 27){
        letter = inb(COM1);
        letter = inb(COM1);
        // checks to see if left arrow key is pressed, if so move cursor position left as long as its greater than 0
        if (letter == 68){
          if (cursor_pos != 0){
            serial_print("\x1b[1D");
            cursor_pos--;
          }
        }
        // checks to see if right arrow key is pressed, if so move cursor position right as long as its less than number of characters
        else if (letter == 67){
          if (cursor_pos != num_characters){
            serial_print("\x1b[1C");
            cursor_pos++;
          }
        }
        // these make sure up and down arrow keys don't print to terminal
        else if (letter == 66){

        }
        else if (letter == 65){

        }
      }
      //stores character into buffer and prints character to the consle and increases cursor posiiton and number of characters
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

