
#ifndef CONFIG_H
#define CONFIG_H

#include "main.h"

/* ========== CAM BIEN DO LINE ========== */
/* S0 ngoai cung phai, S4 ngoai cungtrai */

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

/* ========== MOTOR - 2 TB6612FNG ========== */
/* Kenh A cua ca hai driver: 2 banh trai */
/* Kenh B cua ca hai driver: 2 banh phai */

/* PWM TIM2 */
#define MOTOR_TIM   htim2
#define MOTOR_L_CH  TIM_CHANNEL_1  // PA0
#define MOTOR_R_CH  TIM_CHANNEL_2  // PA1

/* Chieu quay 2 banh trai */
#define L_IN1_PORT GPIOB
#define L_IN1_PIN  GPIO_PIN_0

#define L_IN2_PORT GPIOB
#define L_IN2_PIN  GPIO_PIN_1

/* Chieu quay 2 banh phai */
#define R_IN1_PORT GPIOB
#define R_IN1_PIN  GPIO_PIN_5

#define R_IN2_PORT GPIOB
#define R_IN2_PIN  GPIO_PIN_8

/* STBY chung cua 2 driver */
#define MOTOR_STBY_PORT GPIOB
#define MOTOR_STBY_PIN  GPIO_PIN_9

/* Dao chieu quay neu can */
#define MOTOR_L_INVERT 1
#define MOTOR_R_INVERT 0

/* Bu toc do dong co */
#define TRIM_LEFT  1.00f
#define TRIM_RIGHT 1.00f

/* ========== DIEU KHIEN ========== */

/* Toc do theo thang PWM 0-1000 */
#define BASE_SPEED 300
#define MIN_SPEED  150
#define SLOW_GAIN  20.0f

/* Thong so PID */
#define PID_KP 50.0f
#define PID_KI 0.0f
#define PID_KD 0.4f

/* Chu ky PID 5ms */
#define CTRL_DT 0.005f

/* Xu ly khi mat line */
#define LOST_ERROR 5.0f
#define LOST_TIMEOUT_TICKS 200

/* Thoi gian cho truoc khi chay */
#define START_DELAY_MS 3000

#endif
