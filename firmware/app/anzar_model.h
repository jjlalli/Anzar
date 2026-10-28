#ifndef ANZAR_MODEL_H
#define ANZAR_MODEL_H

#include <stdbool.h>
#include <stdint.h>
#include "anzar_config.h"
#include "anzar_detector.h"

bool anzar_model_init(void);

/* Classifies one window with the NanoEdge library. Writes the winning
   class probability to *confidence. */
anzar_flow_t anzar_model_classify(const int16_t buf[ANZAR_BUFFER_SIZE], float *confidence);

#endif
