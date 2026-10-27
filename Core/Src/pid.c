#include "pid.h"

/* 0..1, smaller = smoother but slower D response */
#define D_FILTER_ALPHA 0.3f

void PID_Init(PID_t *pid, float kp, float ki, float kd,
              float out_min, float out_max)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;

    pid->out_min = out_min;
    pid->out_max = out_max;
    pid->i_max = 300.0f;

    PID_Reset(pid);
}

float PID_Compute(PID_t *pid, float error, float dt)
{
    if (dt <= 0.0f)
    {
        return 0.0f;
    }

    /* I: Accumulate error over time. */
    pid->integral += error * dt;

    if (pid->integral > pid->i_max)
    {
        pid->integral = pid->i_max;
    }
    else if (pid->integral < -pid->i_max)
    {
        pid->integral = -pid->i_max;
    }

    /* D: Skip derivative on the first sample after init/reset. */
    float derivative = 0.0f;
    if (pid->has_prev)
    {
        derivative = (error - pid->prev_error) / dt;
    }

    /* Low-pass filter: sensor error moves in steps, raw D would spike. */
    pid->d_filt += D_FILTER_ALPHA * (derivative - pid->d_filt);
    derivative = pid->d_filt;

    pid->prev_error = error;
    pid->has_prev = 1U;

    float output = pid->Kp * error
                 + pid->Ki * pid->integral
                 + pid->Kd * derivative;

    if (output > pid->out_max)
    {
        output = pid->out_max;
    }
    else if (output < pid->out_min)
    {
        output = pid->out_min;
    }

    return output;
}

void PID_Reset(PID_t *pid)
{
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->d_filt = 0.0f;
    pid->has_prev = 0U;
}
