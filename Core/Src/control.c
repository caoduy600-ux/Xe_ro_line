
#include "control.h"
#include "config.h"
#include "pid.h"
#include "sensor.h"
#include "motor.h"
#include "tim.h"
#include "main.h"

/* ===== BIEN TRANG THAI DIEU KHIEN ===== */
static PID_t pid_line;

static volatile uint8_t running = 0U;

static uint16_t lost_ticks = 0U;

/* +1: line o phai, -1: line o trai, 0: giua */
static int8_t last_side = 0;

/* ===== 1. KHOI TAO HE THONG ===== */

void Control_Init(void)
{
    running = 0U;
    lost_ticks = 0U;
    last_side = 0;

    PID_Init(&pid_line,
             PID_KP, PID_KI, PID_KD,
             -1000.0f, 1000.0f);

    Motor_Init();

    /* Khoi dong ngat TIM1 moi 5 ms */
    if (HAL_TIM_Base_Start_IT(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
}
/* ===== 2. BAT DAU DO LINE ===== */

void Control_Start(void)
{
    float err = 0.0f;

    running = 0U;
    Motor_Stop();

    PID_Reset(&pid_line);

    lost_ticks = 0U;
    last_side = 0;

    /* Kiem tra vach truoc khi khoi dong */
    uint8_t found = Sensor_GetError(&err);

    if (!found || err < -2.0f || err > 2.0f)
    {
        return;
    }

    /* Luu huong line ban dau */
    if (err > 0.0f)
        last_side = 1;
    else if (err < 0.0f)
        last_side = -1;

    running = 1U;
}


/* ===== 3. DUNG XE ===== */

void Control_Stop(void)
{
    running = 0U;
    Motor_Stop();
}


/* ===== 4. DOC TRANG THAI ===== */

uint8_t Control_IsRunning(void)
{
    return running;
}


/* ===== 5. VONG DIEU KHIEN 5 MS ===== */

static void Control_Tick(void)
{
    if (!running)
    {
        return;
    }

    float err = 0.0f;

    uint8_t found = Sensor_GetError(&err);

    /* ===== MAT LINE ===== */

    if (!found)
    {
        /* Xoa trang thai PID o lan mat line dau */
        if (lost_ticks == 0U)
        {
            PID_Reset(&pid_line);
        }

        if (lost_ticks < LOST_TIMEOUT_TICKS)
        {
            lost_ticks++;
        }

        /* Mat line qua lau: dung xe */
        if (lost_ticks >= LOST_TIMEOUT_TICKS)
        {
            Control_Stop();
            return;
        }

        /* Mat line ngan: giu lenh motor cu */
        if (lost_ticks <= LOST_GRACE_TICKS)
        {
            return;
        }

        /* Tim line theo huong cuoi */
        if (last_side > 0)
        {
            Motor_Set((int16_t)MIN_SPEED, 0);
        }
        else if (last_side < 0)
        {
            Motor_Set(0, (int16_t)MIN_SPEED);
        }
        else
        {
            /* Lan cuoi line nam giua:
               tien cham mot khoang ngan */
            if (lost_ticks < LOST_CENTER_STOP_TICKS)
            {
                Motor_Set((int16_t)MIN_SPEED,
                          (int16_t)MIN_SPEED);
            }
            else
            {
                Control_Stop();
            }
        }

        return;
    }

    /* ===== CO LINE ===== */

    lost_ticks = 0U;

    /* Cap nhat huong line */
    if (err > 0.0f)
        last_side = 1;
    else if (err < 0.0f)
        last_side = -1;
    else
        last_side = 0;

    /* ===== TOC DO CO SO ===== */

    float abs_err = (err >= 0.0f) ? err : -err;

    float base = BASE_SPEED - SLOW_GAIN * abs_err;

    if (base < MIN_SPEED)
        base = MIN_SPEED;

    if (base > 1000.0f)
        base = 1000.0f;

    /* ===== TINH PID ===== */

    float corr = PID_Compute(&pid_line, err, CTRL_DT);

    /* Khong dao chieu banh khi dang bam line */
    if (corr > base)
        corr = base;

    if (corr < -base)
        corr = -base;

    /* ===== TINH TOC DO MOTOR ===== */

    float left = base + corr;
    float right = base - corr;

    if (left > 1000.0f)
        left = 1000.0f;

    if (right > 1000.0f)
        right = 1000.0f;

    Motor_Set((int16_t)left, (int16_t)right);
}


/* ===== 6. NGAT TIM1 ===== */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        Control_Tick();
    }
}
