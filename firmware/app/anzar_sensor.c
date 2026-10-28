#include "anzar_sensor.h"
#include "main.h"
#include "lsm6dso_reg.h"

/* LSM6DSO on the X-NUCLEO-IKS01A3 shield, reached over I2C1 (Arduino D14/D15).
   A different shield or accelerometer only changes this file. */
#define SENSOR_I2C_ADDR  LSM6DSO_I2C_ADD_H
#define SENSOR_ODR       LSM6DSO_XL_ODR_1667Hz
#define SENSOR_FS        LSM6DSO_2g

_Static_assert(ANZAR_AXES == 3, "the accelerometer gives 3 axes");

extern I2C_HandleTypeDef hi2c1;

static stmdev_ctx_t dev;

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    return HAL_I2C_Mem_Write(handle, SENSOR_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
                             (uint8_t *)buf, len, 100);
}

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len)
{
    return HAL_I2C_Mem_Read(handle, SENSOR_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
                            buf, len, 100);
}

static void platform_delay(uint32_t ms)
{
    HAL_Delay(ms);
}

bool anzar_sensor_init(void)
{
    uint8_t id = 0;
    uint8_t rst = 1;

    dev.write_reg = platform_write;
    dev.read_reg = platform_read;
    dev.mdelay = platform_delay;
    dev.handle = &hi2c1;

    HAL_Delay(20);
    lsm6dso_device_id_get(&dev, &id);
    if (id != LSM6DSO_ID)
        return false;

    lsm6dso_reset_set(&dev, PROPERTY_ENABLE);
    for (int tries = 0; rst && tries < 100; tries++) {
        HAL_Delay(1);
        lsm6dso_reset_get(&dev, &rst);
    }
    if (rst)
        return false;

    lsm6dso_i3c_disable_set(&dev, LSM6DSO_I3C_DISABLE);
    lsm6dso_block_data_update_set(&dev, PROPERTY_ENABLE);
    lsm6dso_xl_full_scale_set(&dev, SENSOR_FS);
    lsm6dso_xl_data_rate_set(&dev, SENSOR_ODR);
    return true;
}

bool anzar_sensor_read_window(int16_t buf[ANZAR_BUFFER_SIZE])
{
    uint8_t ready;
    int16_t xyz[3];

    for (uint32_t i = 0; i < ANZAR_SIGNAL_LENGTH; i++) {
        uint32_t start = HAL_GetTick();
        do {
            lsm6dso_xl_flag_data_ready_get(&dev, &ready);
            if (HAL_GetTick() - start > 50)   /* a sample is due every 0.6 ms */
                return false;
        } while (!ready);

        lsm6dso_acceleration_raw_get(&dev, xyz);
        buf[3 * i]     = xyz[0];
        buf[3 * i + 1] = xyz[1];
        buf[3 * i + 2] = xyz[2];
    }
    return true;
}
