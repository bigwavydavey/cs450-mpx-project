#ifndef _PCB_USER_COMMANDS_H
#define _PCB_USER_COMMANDS_H

void SuspendPCB(char *processName);
void ResumePCB(char *processName);
void SetPCBPriority(char *processName, int priority);
void ShowPCB(char *processName);
void ShowReady();
void ShowBlocked();
void ShowALl();

#endif