#ifndef MOTOR_H
#define MOTOR_H
#include <stdint.h>
// khoi tao
void Motor_Init(void);
//dieu khien dong co
void Motor_Set(int16_t left, int16_t right);
// tat
void Motor_Stop(void);

#endif
