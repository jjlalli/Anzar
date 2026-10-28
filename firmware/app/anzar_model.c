#include "anzar_model.h"

#if ANZAR_MODE == ANZAR_MODE_DETECT

#include "NanoEdgeAI.h"
#if !ANZAR_NEAI_V5
#include "knowledge.h"
#endif

/* Older libraries expose their sizes; catch a mismatch at compile time. */
#ifdef DATA_INPUT_USER
_Static_assert(DATA_INPUT_USER == ANZAR_SIGNAL_LENGTH, "signal length differs from the library");
#endif
#ifdef AXIS_NUMBER
_Static_assert(AXIS_NUMBER == ANZAR_AXES, "axis count differs from the library");
#endif
#ifdef CLASS_NUMBER
_Static_assert(CLASS_NUMBER == ANZAR_NUM_CLASSES, "class count differs from the library");
#endif

static float input[ANZAR_BUFFER_SIZE];
static float probabilities[ANZAR_NUM_CLASSES];

bool anzar_model_init(void)
{
#if ANZAR_NEAI_V5
    return neai_classification_init() == NEAI_OK;
#else
    return neai_classification_init(knowledge) == NEAI_OK;
#endif
}

anzar_flow_t anzar_model_classify(const int16_t buf[ANZAR_BUFFER_SIZE], float *confidence)
{
#if ANZAR_NEAI_V5
    int id = 0;
#else
    uint16_t id = 0;
#endif

    for (uint32_t i = 0; i < ANZAR_BUFFER_SIZE; i++)
        input[i] = (float)buf[i];

    *confidence = 0.0f;
    if (neai_classification(input, probabilities, &id) != NEAI_OK)
        return ANZAR_FLOW_UNKNOWN;

    for (uint32_t c = 0; c < ANZAR_NUM_CLASSES; c++)
        if (probabilities[c] > *confidence)
            *confidence = probabilities[c];

    switch (id) {
    case ANZAR_CLASS_ID_NO_FLOW: return ANZAR_FLOW_NONE;
    case ANZAR_CLASS_ID_USE:     return ANZAR_FLOW_USE;
    case ANZAR_CLASS_ID_LEAK:    return ANZAR_FLOW_LEAK;
    default:                     return ANZAR_FLOW_UNKNOWN;
    }
}

#else  /* datalog builds do not link the NanoEdge library */

bool anzar_model_init(void)
{
    return true;
}

anzar_flow_t anzar_model_classify(const int16_t buf[ANZAR_BUFFER_SIZE], float *confidence)
{
    (void)buf;
    *confidence = 0.0f;
    return ANZAR_FLOW_UNKNOWN;
}

#endif
