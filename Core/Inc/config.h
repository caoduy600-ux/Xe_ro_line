
#ifndef CONFIG_H
#define CONFIG_H

#include "main.h"

/* ===== CAM BIEN DO LINE ===== */

/* S0 ben phai, S4 ben trai */
#define SENSOR_ACTIVE_LEVEL 0

#define S0_PORT GPIOB
#define S0_PIN  GPIO_PIN_12

#define S1_PORT GPIOB
#define S1_PIN  GPIO_PIN_13

#define S2_PORT GPIOB
#define S2_PIN  GPIO_PIN_14

#define S3_PORT GPIOB
#define S3_PIN  GPIO_PIN_15

#define S4_PORT GPIOA
#define S4_PIN  GPIO_PIN_8


/* ===== MOTOR - 2 TB6612FNG ===== */

/* PWM TIM2 */
#define MOTOR_TIM  htim2

#define MOTOR_L_CH TIM_CHANNEL_1
#define MOTOR_R_CH TIM_CHANNEL_2

/* Kenh motor A */
#define L_IN1_PORT GPIOB
#define L_IN1_PIN  GPIO_PIN_0

#define L_IN2_PORT GPIOB
#define L_IN2_PIN  GPIO_PIN_1

/* Kenh motor B */
#define R_IN1_PORT GPIOB
#define R_IN1_PIN  GPIO_PIN_5

#define R_IN2_PORT GPIOB
#define R_IN2_PIN  GPIO_PIN_8

/* STBY chung */
#define MOTOR_STBY_PORT GPIOB
#define MOTOR_STBY_PIN  GPIO_PIN_9


/* ===== CHIEU QUAY ===== */

#define MOTOR_L_INVERT 1
#define MOTOR_R_INVERT 0

/* Bu toc do */
#define TRIM_LEFT  1.00f
#define TRIM_RIGHT 1.00f


/* ===== TOC DO DO LINE ===== */

/* PWM: 0 - 1000 */
#define BASE_SPEED 260
#define MIN_SPEED  210

/* Giam toc khi line lech */
#define SLOW_GAIN 15.0f


/* ===== THONG SO PID ===== */

#define PID_KP 50.0f
#define PID_KI 0.0f
#define PID_KD 0.0f

/* Chu ky 5 ms */
#define CTRL_DT 0.005f


/* ===== XU LY MAT LINE ===== */

#define LOST_ERROR 5.0f

/* Cho mat line ngan: 10 ms */
#define LOST_GRACE_TICKS 2U

/* Mat line o giua: 125 ms */
#define LOST_CENTER_STOP_TICKS 25U

/* Mat line toi da: 500 ms */
#define LOST_TIMEOUT_TICKS 100U

/* Giu lai de tuong thich */
#define RECOVER_SPEED 250


/* ===== RE CUA GAP 90 DO ===== */

/* PWM quay tai cho */
#define TURN_SPEED 350

/* Sai so bat dau xoay */
#define TURN_TRIGGER_ERROR 3.0f

/* Quay toi thieu 40 ms */
#define TURN_MIN_TICKS 8U

/* Xac nhan line giua 3 lan */
#define TURN_CONFIRM_TICKS 3U

/* Dung neu quay qua 800 ms */
#define TURN_TIMEOUT_TICKS 160U


/* ===== KHOI DONG ===== */

#define START_DELAY_MS 3000U

#endif
