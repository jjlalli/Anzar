#include <stdio.h>
#include "anzar_app.h"
#include "anzar_config.h"
#include "anzar_sensor.h"
#include "anzar_model.h"
#include "anzar_detector.h"
#include "anzar_io.h"
#include "main.h"

static int16_t window[ANZAR_BUFFER_SIZE];

#if ANZAR_MODE == ANZAR_MODE_DETECT
static anzar_detector_t detector;
#endif

void anzar_app_init(void)
{
    anzar_io_print("# Anzar\r\n");
    if (!anzar_sensor_init())
        anzar_io_fatal("accelerometer not found");

#if ANZAR_MODE == ANZAR_MODE_DETECT
    if (!anzar_model_init())
        anzar_io_fatal("NanoEdge library failed to initialise");
    anzar_detector_init(&detector, HAL_GetTick());
    anzar_io_print("# mode: detect\r\n");
#else
    anzar_io_print("# mode: datalog\r\n");
#endif
}

static void read_window(void)
{
    if (!anzar_sensor_read_window(window))
        anzar_io_fatal("accelerometer stopped responding");
}

#if ANZAR_MODE == ANZAR_MODE_DATALOG

/* One window per line, comma separated: the format NanoEdge AI Studio imports. */
static void print_window(void)
{
    char num[12];

    for (uint32_t i = 0; i < ANZAR_BUFFER_SIZE; i++) {
        int n = snprintf(num, sizeof num, i ? ",%d" : "%d", window[i]);
        anzar_io_write(num, (uint16_t)n);
    }
    anzar_io_write("\r\n", 2);
}

void anzar_app_loop(void)
{
    read_window();
    print_window();
}

#else

static const char *const flow_name[] = {"none", "use", "leak", "unknown"};
static const char *const alert_name[] = {"none", "leak", "tap_left_running"};

/* Leak: LED on. Tap left running: LED blinks. */
static void show(anzar_alert_t alert)
{
    static bool blink;

    if (alert == ANZAR_ALERT_TAP_RUNNING)
        blink = !blink;
    anzar_io_led(alert == ANZAR_ALERT_LEAK || (alert == ANZAR_ALERT_TAP_RUNNING && blink));
}

void anzar_app_loop(void)
{
    float p;

    read_window();
    anzar_flow_t flow = anzar_model_classify(window, &p);
    anzar_alert_t raised = anzar_detector_update(&detector, flow, p, HAL_GetTick());

    if (raised != ANZAR_ALERT_NONE) {
        char line[48];
        int n = snprintf(line, sizeof line, "# ALERT %s\r\n", alert_name[raised]);
        anzar_io_write(line, (uint16_t)n);
        anzar_io_buzz(raised == ANZAR_ALERT_LEAK ? 1000 : 300);
    }
    show(anzar_detector_alert(&detector));

#if ANZAR_VERBOSE
    char status[80];
    int n = snprintf(status, sizeof status, "# window=%s p=%d%% state=%s alert=%s\r\n",
                     flow_name[flow], (int)(p * 100.0f),
                     flow_name[anzar_detector_state(&detector)],
                     alert_name[anzar_detector_alert(&detector)]);
    anzar_io_write(status, (uint16_t)n);
#endif
}

#endif
