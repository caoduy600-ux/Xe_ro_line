
#include "control.h"
#include "config.h"
#include "pid.h"
#include "sensor.h"
#include "motor.h"
#include "tim.h"
#include "main.h"

/* ===== BIEN TRANG THAI ===== */

static PID_t pid_line;

static volatile uint8_t running = 0U;

static uint16_t lost_ticks = 0U;

/* +1: line phai, -1: line trai, 0: giua */
static int8_t last_side = 0;

/* Trang thai re cua gap */
static int8_t turn_dir = 0;
static uint16_t turn_ticks = 0U;
static uint8_t turn_confirm = 0U;
static uint16_t center_ticks = 0U;


/* ===== KHOI TAO ===== */

void Control_Init(void)
{
    running = 0U;
    lost_ticks = 0U;

    last_side = 0;
    turn_dir = 0;
    turn_ticks = 0U;
    turn_confirm = 0U;
    center_ticks = 0U;

    /* Khoi tao PID */
    PID_Init(&pid_line,
             PID_KP,
             PID_KI,
             PID_KD,
             -1000.0f,
             1000.0f);

    /* Khoi tao dong co */
    Motor_Init();

    /* Bat ngat TIM1 moi 5 ms */
    if (HAL_TIM_Base_Start_IT(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
}


/* ===== BAT DAU CHAY ===== */

void Control_Start(void)
{
    float err = 0.0f;

    running = 0U;
    Motor_Stop();

    PID_Reset(&pid_line);

    lost_ticks = 0U;
    last_side = 0;

    turn_dir = 0;
    turn_ticks = 0U;
    turn_confirm = 0U;
    center_ticks = 0U;

    /* Kiem tra line ban dau */
    uint8_t found = Sensor_GetError(&err);

    /* Chi bat dau khi line gan giua */
    if (!found || err < -2.0f || err > 2.0f)
    {
        return;
    }

    /* Luu huong ban dau */
    if (err > 0.0f)
    {
        last_side = 1;
    }
    else if (err < 0.0f)
    {
        last_side = -1;
    }

    running = 1U;
}


/* ===== DUNG XE ===== */

void Control_Stop(void)
{
    running = 0U;
    Motor_Stop();
}


/* ===== DOC TRANG THAI ===== */

uint8_t Control_IsRunning(void)
{
    return running;
}


/* ===== XOAY TAI CHO ===== */

static void Control_Pivot(int8_t direction)
{
    if (direction > 0)
    {
        /* Re phai: trai tien, phai lui */
        Motor_Set((int16_t)TURN_SPEED,
                  -(int16_t)TURN_SPEED);
    }
    else if (direction < 0)
    {
        /* Re trai: trai lui, phai tien */
        Motor_Set(-(int16_t)TURN_SPEED,
                   (int16_t)TURN_SPEED);
    }
}


/* ===== VONG DIEU KHIEN 5 MS ===== */

static void Control_Tick(void)
{
    /* Chua cho phep chay */
    if (!running)
    {
        return;
    }

    float err = 0.0f;

    /* Doc cam bien */
    uint8_t found = Sensor_GetError(&err);


    /* ===== 1. DANG RE CUA GAP ===== */

    if (turn_dir != 0)
    {
        turn_ticks++;

        /* Quay qua lau thi dung */
        if (turn_ticks >= TURN_TIMEOUT_TICKS)
        {
            Control_Stop();
            return;
        }

        /* Kiem tra line da ve giua */
        if (turn_ticks >= TURN_MIN_TICKS &&
            found &&
            err >= -1.0f &&
            err <= 1.0f)
        {
            turn_confirm++;

            /* Xac nhan bat lai line */
            if (turn_confirm >= TURN_CONFIRM_TICKS)
            {
                turn_dir = 0;
                turn_ticks = 0U;
                turn_confirm = 0U;

                lost_ticks = 0U;
                last_side = 0;
                center_ticks = 0U;

                PID_Reset(&pid_line);
            }
            else
            {
                /* Tiep tuc xoay de xac nhan */
                Control_Pivot(turn_dir);
                return;
            }
        }
        else
        {
            turn_confirm = 0U;

            /* Chua bat lai line */
            Control_Pivot(turn_dir);
            return;
        }
    }


    /* ===== 2. MAT LINE ===== */

    if (!found)
    {
        /* Reset PID mot lan */
        if (lost_ticks == 0U)
        {
            PID_Reset(&pid_line);
        }

        if (lost_ticks < LOST_TIMEOUT_TICKS)
        {
            lost_ticks++;
        }

        /* Mat line qua lau */
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

        /* Co huong cuoi: xoay tim line */
        if (last_side != 0)
        {
            turn_dir = last_side;

            turn_ticks = 0U;
            turn_confirm = 0U;

            Control_Pivot(turn_dir);
            return;
        }

        /* Khong biet huong: dung sau mot doan */
        if (lost_ticks >= LOST_CENTER_STOP_TICKS)
        {
            Control_Stop();
            return;
        }

        /* Tien cham de tim lai line */
        Motor_Set((int16_t)MIN_SPEED,
                  (int16_t)MIN_SPEED);

        return;
    }


    /* ===== 3. CO LINE ===== */

    lost_ticks = 0U;

    /* Ghi nho huong line */
    if (err > 0.5f)
    {
        last_side = 1;
        center_ticks = 0U;
    }
    else if (err < -0.5f)
    {
        last_side = -1;
        center_ticks = 0U;
    }
    else
    {
        /* O giua lau thi xoa huong cu */
        if (center_ticks < 20U)
        {
            center_ticks++;
        }

        if (center_ticks >= 20U)
        {
            last_side = 0;
        }
    }


    /* ===== 4. PHAT HIEN CUA GAP ===== */

    if (err >= TURN_TRIGGER_ERROR ||
        err <= -TURN_TRIGGER_ERROR)
    {
        /* Xac dinh huong cua */
        turn_dir = (err > 0.0f) ? 1 : -1;

        turn_ticks = 0U;
        turn_confirm = 0U;

        PID_Reset(&pid_line);

        /* Bat dau xoay tai cho */
        Control_Pivot(turn_dir);
        return;
    }


    /* ===== 5. TOC DO CO SO ===== */

    float abs_err = (err >= 0.0f) ? err : -err;

    float base = BASE_SPEED - SLOW_GAIN * abs_err;

    if (base < MIN_SPEED)
    {
        base = MIN_SPEED;
    }

    if (base > 1000.0f)
    {
        base = 1000.0f;
    }


    /* ===== 6. TINH PID ===== */

    float corr = PID_Compute(&pid_line,
                             err,
                             CTRL_DT);

    /* Gioi han hieu chinh */
    if (corr > base)
    {
        corr = base;
    }

    if (corr < -base)
    {
        corr = -base;
    }


    /* ===== 7. TINH TOC DO DONG CO ===== */

    float left = base + corr;
    float right = base - corr;

    /* Gioi han PWM */
    if (left > 1000.0f)
    {
        left = 1000.0f;
    }

    if (right > 1000.0f)
    {
        right = 1000.0f;
    }

    /* Cap nhat toc do dong co */
    Motor_Set((int16_t)left,
              (int16_t)right);
}


/* ===== CALLBACK NGAT TIM1 ===== */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        Control_Tick();
    }
}
