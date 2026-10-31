
#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>

void Control_Init(void);

void Control_Start(void);

void Control_Stop(void);

uint8_t Control_IsRunning(void);

#endif
