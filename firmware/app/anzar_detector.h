#ifndef ANZAR_DETECTOR_H
#define ANZAR_DETECTOR_H

#include <stdint.h>
#include "anzar_config.h"

typedef enum {
    ANZAR_FLOW_NONE = 0,
    ANZAR_FLOW_USE,
    ANZAR_FLOW_LEAK,
    ANZAR_FLOW_UNKNOWN
} anzar_flow_t;

typedef enum {
    ANZAR_ALERT_NONE = 0,
    ANZAR_ALERT_LEAK,
    ANZAR_ALERT_TAP_RUNNING
} anzar_alert_t;

typedef struct {
    anzar_flow_t votes[ANZAR_VOTE_WINDOW];
    uint8_t next;
    uint8_t count;
    anzar_flow_t state;
    uint32_t since_ms;
    anzar_alert_t alert;
} anzar_detector_t;

void anzar_detector_init(anzar_detector_t *d, uint32_t now_ms);

/* Feed one classified window. Returns an alert only on the window that raises it;
   the current alert stays available through anzar_detector_alert(). */
anzar_alert_t anzar_detector_update(anzar_detector_t *d, anzar_flow_t flow,
                                    float confidence, uint32_t now_ms);

anzar_alert_t anzar_detector_alert(const anzar_detector_t *d);
anzar_flow_t anzar_detector_state(const anzar_detector_t *d);

#endif
