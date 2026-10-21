#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/* Bit i = 1 nếu mắt i thấy line (bit0 = trái nhất) */
uint8_t Sensor_ReadMask(void);

/* Tính sai số vị trí (-4..+4, dương = line lệch phải).
   Trả về 1 nếu thấy line, 0 nếu mất line (khi đó *err là sai số giả theo hướng cũ). */
uint8_t Sensor_GetError(float *err);

#endif
