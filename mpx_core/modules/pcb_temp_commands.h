#ifndef _PCB_TEMP_COMMANDS_H
#define _PCB_TEMP_COMMANDS_H

void CreatePCB(char *processName, int class, int priority);
void DeletePCB(char *processName);
void BlockPCB(char *processName);
void UnblockPCB(char *processName);

#endif