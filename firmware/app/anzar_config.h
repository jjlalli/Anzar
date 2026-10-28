#ifndef ANZAR_CONFIG_H
#define ANZAR_CONFIG_H

/* Build mode: record training data first, then switch to detection. */
#define ANZAR_MODE_DATALOG  0
#define ANZAR_MODE_DETECT   1
#ifndef ANZAR_MODE
#define ANZAR_MODE          ANZAR_MODE_DATALOG
#endif

/* One window = ANZAR_SIGNAL_LENGTH samples of 3 axes, interleaved x,y,z.
   Must match the signal settings of the NanoEdge AI Studio project. */
#define ANZAR_AXES           3
#define ANZAR_SIGNAL_LENGTH  512
#define ANZAR_BUFFER_SIZE    (ANZAR_SIGNAL_LENGTH * ANZAR_AXES)

/* NanoEdge library. 1 for Studio v5 libraries (init takes no argument),
   0 for older libraries that come with a knowledge.h file. */
#ifndef ANZAR_NEAI_V5
#define ANZAR_NEAI_V5        1
#endif
#define ANZAR_NUM_CLASSES    3

/* Class IDs exactly as the generated library reports them (docs/training.md). */
#define ANZAR_CLASS_ID_NO_FLOW  1
#define ANZAR_CLASS_ID_USE      2
#define ANZAR_CLASS_ID_LEAK     3

/* Decision logic. */
#define ANZAR_MIN_CONFIDENCE    0.60f
#define ANZAR_VOTE_WINDOW       5
#define ANZAR_LEAK_CONFIRM_MS   (30UL * 60UL * 1000UL)   /* faint flow that never stops */
#define ANZAR_TAP_RUNNING_MS    (20UL * 60UL * 1000UL)   /* strong flow that never stops */

/* I/O. */
#ifndef ANZAR_USE_BUZZER
#define ANZAR_USE_BUZZER        0   /* needs a GPIO output labelled BUZZER in CubeMX */
#endif
#ifndef ANZAR_VERBOSE
#define ANZAR_VERBOSE           1   /* one status line per window on the serial port */
#endif

#endif
