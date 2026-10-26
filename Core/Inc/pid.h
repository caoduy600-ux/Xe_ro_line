#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct
{
    float Kp;
    float Ki;
    float Kd;

    float integral;
    float prev_error;

    float out_min;
    float out_max;
    float i_max;

    uint8_t has_prev;
} PID_t;

void PID_Init(PID_t *pid, float kp, float ki, float kd,
              float out_min, float out_max);

float PID_Compute(PID_t *pid, float error, float dt);

void PID_Reset(PID_t *pid);

#endif /* PID_H */
