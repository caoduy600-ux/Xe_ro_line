
#include "motor.h"
#include "config.h"
#include "tim.h"

/* Dung tat ca dong co */
void Motor_Stop(void)
{
    /* 1. Dua hai driver ve Standby */
    HAL_GPIO_WritePin(
        MOTOR_STBY_PORT,
        MOTOR_STBY_PIN,
        GPIO_PIN_RESET
    );

    /* 2. Dua hai kenh PWM ve 0 */
    __HAL_TIM_SET_COMPARE(&MOTOR_TIM, MOTOR_L_CH, 0);
    __HAL_TIM_SET_COMPARE(&MOTOR_TIM, MOTOR_R_CH, 0);

    /* 3. Reset cac chan chon chieu quay */
    HAL_GPIO_WritePin(L_IN1_PORT, L_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L_IN2_PORT, L_IN2_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(R_IN1_PORT, R_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(R_IN2_PORT, R_IN2_PIN, GPIO_PIN_RESET);
}

/* Khoi tao he thong dong co */
void Motor_Init(void)
{
    /* Dua dong co ve trang thai an toan */
    Motor_Stop();
    /* Bat PWM kenh trai - PA0 */
    if (HAL_TIM_PWM_Start(&MOTOR_TIM, MOTOR_L_CH) != HAL_OK)
    {
        Error_Handler();
    }
    /* Bat PWM kenh phai - PA1 */
    if (HAL_TIM_PWM_Start(&MOTOR_TIM, MOTOR_R_CH) != HAL_OK)
    {
        Error_Handler();
    }
}
void Motor_Set(int16_t left, int16_t right)
{
    //tu dat gia tri neu 2 banh lech nhau
    int32_t left_speed  = (int32_t)((float)left * TRIM_LEFT);
    int32_t right_speed = (int32_t)((float)right * TRIM_RIGHT);

    /* 2. Dao chieu theo cau hinh */
    if (MOTOR_L_INVERT)
        left_speed = -left_speed;

    if (MOTOR_R_INVERT)
        right_speed = -right_speed;

    /* 3. Gioi han toc do */
    if (left_speed > 1000) left_speed = 1000;
    if (left_speed < -1000) left_speed = -1000;

    if (right_speed > 1000) right_speed = 1000;
    if (right_speed < -1000) right_speed = -1000;

    /* 4. Neu ca hai ben bang 0 thi dung */
    if (left_speed == 0 && right_speed == 0)
    {
        Motor_Stop();
        return;
    }

    /* 5. Tam tat driver trong luc cap nhat */
    HAL_GPIO_WritePin(MOTOR_STBY_PORT, MOTOR_STBY_PIN,
                      GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&MOTOR_TIM, MOTOR_L_CH, 0);
    __HAL_TIM_SET_COMPARE(&MOTOR_TIM, MOTOR_R_CH, 0);

    /* 6. Chon chieu quay ben trai */
    if (left_speed > 0)
    {
        HAL_GPIO_WritePin(L_IN1_PORT, L_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(L_IN2_PORT, L_IN2_PIN, GPIO_PIN_RESET);
    }
    else if (left_speed < 0)
    {
        HAL_GPIO_WritePin(L_IN1_PORT, L_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(L_IN2_PORT, L_IN2_PIN, GPIO_PIN_SET);

        left_speed = -left_speed;
    }
    else
    {
        HAL_GPIO_WritePin(L_IN1_PORT, L_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(L_IN2_PORT, L_IN2_PIN, GPIO_PIN_RESET);
    }

    /* 7. Chon chieu quay ben phai */
    if (right_speed > 0)
    {
        HAL_GPIO_WritePin(R_IN1_PORT, R_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(R_IN2_PORT, R_IN2_PIN, GPIO_PIN_RESET);
    }
    else if (right_speed < 0)
    {
        HAL_GPIO_WritePin(R_IN1_PORT, R_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(R_IN2_PORT, R_IN2_PIN, GPIO_PIN_SET);

        right_speed = -right_speed;
    }
    else
    {
        HAL_GPIO_WritePin(R_IN1_PORT, R_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(R_IN2_PORT, R_IN2_PIN, GPIO_PIN_RESET);
    }

    /* 8. Chuyen toc do thanh CCR */
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(&MOTOR_TIM) + 1U;

    uint32_t left_pwm =
        (uint32_t)left_speed * period / 1000U;

    uint32_t right_pwm =
        (uint32_t)right_speed * period / 1000U;

    /* 9. Ghi gia tri CCR */
    __HAL_TIM_SET_COMPARE(&MOTOR_TIM, MOTOR_L_CH, left_pwm);
    __HAL_TIM_SET_COMPARE(&MOTOR_TIM, MOTOR_R_CH, right_pwm);

    /* 10. Cho phep hai driver hoat dong */
    HAL_GPIO_WritePin(MOTOR_STBY_PORT, MOTOR_STBY_PIN,
                      GPIO_PIN_SET);
}
