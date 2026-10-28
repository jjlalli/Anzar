#include "anzar_detector.h"

void anzar_detector_init(anzar_detector_t *d, uint32_t now_ms)
{
    d->next = 0;
    d->count = 0;
    d->state = ANZAR_FLOW_NONE;
    d->since_ms = now_ms;
    d->alert = ANZAR_ALERT_NONE;
}

/* A class wins only with a strict majority of the full vote window. */
static anzar_flow_t majority(const anzar_detector_t *d)
{
    uint8_t n[ANZAR_FLOW_UNKNOWN] = {0};

    for (uint8_t i = 0; i < d->count; i++)
        if (d->votes[i] != ANZAR_FLOW_UNKNOWN)
            n[d->votes[i]]++;

    for (uint8_t c = 0; c < ANZAR_FLOW_UNKNOWN; c++)
        if (2 * n[c] > ANZAR_VOTE_WINDOW)
            return (anzar_flow_t)c;

    return ANZAR_FLOW_UNKNOWN;
}

anzar_alert_t anzar_detector_update(anzar_detector_t *d, anzar_flow_t flow,
                                    float confidence, uint32_t now_ms)
{
    if (confidence < ANZAR_MIN_CONFIDENCE)
        flow = ANZAR_FLOW_UNKNOWN;

    d->votes[d->next] = flow;
    d->next = (uint8_t)((d->next + 1) % ANZAR_VOTE_WINDOW);
    if (d->count < ANZAR_VOTE_WINDOW)
        d->count++;

    anzar_flow_t s = majority(d);
    if (s != d->state) {
        d->state = s;
        d->since_ms = now_ms;
        if (s == ANZAR_FLOW_NONE)
            d->alert = ANZAR_ALERT_NONE;   /* the pipe went quiet */
    }

    uint32_t held = now_ms - d->since_ms;  /* unsigned: survives tick wraparound */
    anzar_alert_t want = ANZAR_ALERT_NONE;

    if (d->state == ANZAR_FLOW_LEAK && held >= ANZAR_LEAK_CONFIRM_MS)
        want = ANZAR_ALERT_LEAK;
    else if (d->state == ANZAR_FLOW_USE && held >= ANZAR_TAP_RUNNING_MS)
        want = ANZAR_ALERT_TAP_RUNNING;

    if (want == ANZAR_ALERT_NONE || want == d->alert)
        return ANZAR_ALERT_NONE;

    d->alert = want;
    return want;
}

anzar_alert_t anzar_detector_alert(const anzar_detector_t *d)
{
    return d->alert;
}

anzar_flow_t anzar_detector_state(const anzar_detector_t *d)
{
    return d->state;
}
