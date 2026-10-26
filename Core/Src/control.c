#include "control.h"
#include "config.h"
#include "pid.h"
#include "sensor.h"
#include "motor.h"
#include "tim.h"
#include "main.h"

static PID_t pid_line;
static volatile uint8_t running = 0U;
static uint16_t lost_ticks = 0U;

volatile int dbg_err100 = 0;
volatile int dbg_corr = 0;
volatile uint8_t dbg_found = 0U;
volatile uint16_t dbg_lost_ticks = 0U;

/* Called after MX_TIM1_Init(), MX_TIM2_Init() and MX_GPIO_Init(). */
void Control_Init(void)
{
    running = 0U;
    lost_ticks = 0U;

    PID_Init(&pid_line, PID_KP, PID_KI, PID_KD, -1000.0f, 1000.0f);
    Motor_Init();

    if (HAL_TIM_Base_Start_IT(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
}

void Control_Start(void)
{
    float err = 0.0f;

    running = 0U;
    Motor_Stop();
    PID_Reset(&pid_line);
    lost_ticks = 0U;
    dbg_lost_ticks = 0U;
    dbg_corr = 0;

    /* Don't launch blind or while the line is far from the middle. */
    dbg_found = Sensor_GetError(&err);
    dbg_err100 = (int)(err * 100.0f);
    if (!dbg_found || err < -2.0f || err > 2.0f)
    {
        return;
    }

    running = 1U;
}

void Control_Stop(void)
{
    running = 0U;
    Motor_Stop();
}

uint8_t Control_IsRunning(void)
{
    return running;
}

/* Call exactly once per TIM1 update interrupt (5 ms). */
static void Control_Tick(void)
{
    if (!running)
    {
        return;
    }

    float err = 0.0f;
    uint8_t found = Sensor_GetError(&err);
    dbg_found = found;
    dbg_err100 = (int)(err * 100.0f);

    if (!found)
    {
        /* The sensor returns a synthetic +/-LOST_ERROR: never feed it to PID. */
        PID_Reset(&pid_line);
        dbg_corr = 0;
        ++lost_ticks;
        dbg_lost_ticks = lost_ticks;

        if (err == 0.0f || lost_ticks >= LOST_TIMEOUT_TICKS)
        {
            Control_Stop();
            return;
        }

        /* Short, slow recovery toward last known line position. */
        if (err > 0.0f)
        {
            Motor_Set((int16_t)MIN_SPEED, 0);
        }
        else
        {
            Motor_Set(0, (int16_t)MIN_SPEED);
        }
        return;
    }

    lost_ticks = 0U;
    dbg_lost_ticks = 0U;

    float abs_err = (err >= 0.0f) ? err : -err;
    float base = BASE_SPEED - SLOW_GAIN * abs_err;
    if (base < MIN_SPEED) base = MIN_SPEED;

    float corr = PID_Compute(&pid_line, err, CTRL_DT);

    /* Keep both motor commands non-negative in the initial line-following test. */
    if (corr > base) corr = base;
    if (corr < -base) corr = -base;

    float left = base + corr;
    float right = base - corr;
    if (left > 1000.0f) left = 1000.0f;
    if (right > 1000.0f) right = 1000.0f;

    dbg_corr = (int)corr;
    Motor_Set((int16_t)left, (int16_t)right);
}

/* Define this HAL callback only once in the whole project. */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        Control_Tick();
    }
}
