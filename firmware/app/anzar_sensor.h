#ifndef ANZAR_SENSOR_H
#define ANZAR_SENSOR_H

#include <stdbool.h>
#include <stdint.h>
#include "anzar_config.h"

bool anzar_sensor_init(void);

/* Fills buf with one window, axes interleaved: x0, y0, z0, x1, y1, z1, ...
   Returns false if the sensor stops delivering samples. */
bool anzar_sensor_read_window(int16_t buf[ANZAR_BUFFER_SIZE]);

#endif
