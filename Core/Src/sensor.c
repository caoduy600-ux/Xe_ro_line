#include "sensor.h"
#include "config.h"

static float last_error = 0.0f;

uint8_t Sensor_ReadMask(void)
{
    uint8_t m = 0;
    if (HAL_GPIO_ReadPin(S0_PORT, S0_PIN) == SENSOR_ACTIVE_LEVEL) m |= (1 << 0);
    if (HAL_GPIO_ReadPin(S1_PORT, S1_PIN) == SENSOR_ACTIVE_LEVEL) m |= (1 << 1);
    if (HAL_GPIO_ReadPin(S2_PORT, S2_PIN) == SENSOR_ACTIVE_LEVEL) m |= (1 << 2);
    if (HAL_GPIO_ReadPin(S3_PORT, S3_PIN) == SENSOR_ACTIVE_LEVEL) m |= (1 << 3);
    if (HAL_GPIO_ReadPin(S4_PORT, S4_PIN) == SENSOR_ACTIVE_LEVEL) m |= (1 << 4);
    return m;
}

uint8_t Sensor_GetError(float *err)
{
    static const int w[5] = {-4, -2, 0, 2, 4};
    uint8_t m = Sensor_ReadMask();
    int sum = 0, cnt = 0;

    for (int i = 0; i < 5; i++) {
        if (m & (1 << i)) { sum += w[i]; cnt++; }
    }

    if (cnt == 0) {                      /* mất line: đẩy mạnh về phía lần cuối thấy */
        if (last_error > 0)      *err =  LOST_ERROR;
        else if (last_error < 0) *err = -LOST_ERROR;
        else                     *err = 0;
        return 0;
    }

    last_error = (float)sum / (float)cnt;
    *err = last_error;
    return 1;
}
