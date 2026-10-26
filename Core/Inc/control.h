#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>

void Control_Init(void);
void Control_Start(void);
void Control_Stop(void);
uint8_t Control_IsRunning(void);

/* Debug: err x100, PID correction, line detected, lost-line ticks */
extern volatile int dbg_err100;
extern volatile int dbg_corr;
extern volatile uint8_t dbg_found;
extern volatile uint16_t dbg_lost_ticks;

#endif /* CONTROL_H */
