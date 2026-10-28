#include <stdio.h>
#include <stdint.h>
#include "anzar_detector.h"

#define WINDOW_MS 500u
#define MIN_MS (60u * 1000u)

static int failures;

#define CHECK(cond) do { \
    if (!(cond)) { printf("FAIL %s:%d  %s\n", __func__, __LINE__, #cond); failures++; } \
} while (0)

/* Feeds `ms` worth of identical windows; returns how many alerts of `kind` were raised. */
static int feed(anzar_detector_t *d, uint32_t *t, anzar_flow_t flow, float p,
                uint32_t ms, anzar_alert_t kind)
{
    int raised = 0;
    for (uint32_t end = *t + ms; *t != end; *t += WINDOW_MS)
        if (anzar_detector_update(d, flow, p, *t) == kind)
            raised++;
    return raised;
}

static void quiet_pipe_never_alerts(void)
{
    anzar_detector_t d;
    uint32_t t = 0;
    anzar_detector_init(&d, t);
    CHECK(feed(&d, &t, ANZAR_FLOW_NONE, 0.95f, 120 * MIN_MS, ANZAR_ALERT_LEAK) == 0);
    CHECK(anzar_detector_alert(&d) == ANZAR_ALERT_NONE);
}

static void short_use_does_not_alert(void)
{
    anzar_detector_t d;
    uint32_t t = 0;
    anzar_detector_init(&d, t);
    CHECK(feed(&d, &t, ANZAR_FLOW_USE, 0.9f, 10 * MIN_MS, ANZAR_ALERT_TAP_RUNNING) == 0);
    feed(&d, &t, ANZAR_FLOW_NONE, 0.9f, MIN_MS, ANZAR_ALERT_NONE);
    CHECK(anzar_detector_alert(&d) == ANZAR_ALERT_NONE);
}

static void tap_left_running_alerts_once_then_clears(void)
{
    anzar_detector_t d;
    uint32_t t = 0;
    anzar_detector_init(&d, t);
    CHECK(feed(&d, &t, ANZAR_FLOW_USE, 0.9f, 19 * MIN_MS, ANZAR_ALERT_TAP_RUNNING) == 0);
    CHECK(feed(&d, &t, ANZAR_FLOW_USE, 0.9f, 10 * MIN_MS, ANZAR_ALERT_TAP_RUNNING) == 1);
    CHECK(anzar_detector_alert(&d) == ANZAR_ALERT_TAP_RUNNING);
    feed(&d, &t, ANZAR_FLOW_NONE, 0.9f, MIN_MS, ANZAR_ALERT_NONE);
    CHECK(anzar_detector_alert(&d) == ANZAR_ALERT_NONE);
}

static void leak_needs_the_full_duration(void)
{
    anzar_detector_t d;
    uint32_t t = 0;
    anzar_detector_init(&d, t);
    CHECK(feed(&d, &t, ANZAR_FLOW_LEAK, 0.8f, 29 * MIN_MS, ANZAR_ALERT_LEAK) == 0);
    CHECK(feed(&d, &t, ANZAR_FLOW_LEAK, 0.8f, 5 * MIN_MS, ANZAR_ALERT_LEAK) == 1);
    CHECK(anzar_detector_alert(&d) == ANZAR_ALERT_LEAK);
}

static void use_interrupting_a_leak_restarts_the_timer(void)
{
    anzar_detector_t d;
    uint32_t t = 0;
    anzar_detector_init(&d, t);
    feed(&d, &t, ANZAR_FLOW_LEAK, 0.8f, 25 * MIN_MS, ANZAR_ALERT_NONE);
    feed(&d, &t, ANZAR_FLOW_USE, 0.9f, 2 * MIN_MS, ANZAR_ALERT_NONE);
    CHECK(feed(&d, &t, ANZAR_FLOW_LEAK, 0.8f, 25 * MIN_MS, ANZAR_ALERT_LEAK) == 0);
    CHECK(feed(&d, &t, ANZAR_FLOW_LEAK, 0.8f, 10 * MIN_MS, ANZAR_ALERT_LEAK) == 1);
}

static void one_wrong_window_does_not_reset_the_timer(void)
{
    anzar_detector_t d;
    uint32_t t = 0;
    anzar_detector_init(&d, t);
    feed(&d, &t, ANZAR_FLOW_USE, 0.9f, 15 * MIN_MS, ANZAR_ALERT_NONE);
    anzar_detector_update(&d, ANZAR_FLOW_NONE, 0.9f, t);
    t += WINDOW_MS;
    CHECK(anzar_detector_state(&d) == ANZAR_FLOW_USE);
    CHECK(feed(&d, &t, ANZAR_FLOW_USE, 0.9f, 6 * MIN_MS, ANZAR_ALERT_TAP_RUNNING) == 1);
}

static void low_confidence_never_builds_an_alert(void)
{
    anzar_detector_t d;
    uint32_t t = 0;
    anzar_detector_init(&d, t);
    CHECK(feed(&d, &t, ANZAR_FLOW_LEAK, 0.4f, 60 * MIN_MS, ANZAR_ALERT_LEAK) == 0);
    CHECK(anzar_detector_state(&d) == ANZAR_FLOW_UNKNOWN);
}

static void survives_tick_wraparound(void)
{
    anzar_detector_t d;
    uint32_t t = UINT32_MAX - 10u * MIN_MS + 1u;
    anzar_detector_init(&d, t);
    CHECK(feed(&d, &t, ANZAR_FLOW_USE, 0.9f, 25 * MIN_MS, ANZAR_ALERT_TAP_RUNNING) == 1);
}

static void tap_alert_can_follow_a_leak_alert(void)
{
    anzar_detector_t d;
    uint32_t t = 0;
    anzar_detector_init(&d, t);
    CHECK(feed(&d, &t, ANZAR_FLOW_LEAK, 0.8f, 35 * MIN_MS, ANZAR_ALERT_LEAK) == 1);
    CHECK(feed(&d, &t, ANZAR_FLOW_USE, 0.9f, 25 * MIN_MS, ANZAR_ALERT_TAP_RUNNING) == 1);
}

int main(void)
{
    quiet_pipe_never_alerts();
    short_use_does_not_alert();
    tap_left_running_alerts_once_then_clears();
    leak_needs_the_full_duration();
    use_interrupting_a_leak_restarts_the_timer();
    one_wrong_window_does_not_reset_the_timer();
    low_confidence_never_builds_an_alert();
    survives_tick_wraparound();
    tap_alert_can_follow_a_leak_alert();

    if (failures) {
        printf("%d check(s) failed\n", failures);
        return 1;
    }
    printf("all detector tests passed\n");
    return 0;
}
