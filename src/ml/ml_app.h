#ifndef ML_APP_H
#define ML_APP_H

#include <stdint.h>
#include <stdbool.h>

/* Class indices as reported by the Knowledge Pack (model.json ClassMaps) */
typedef enum {
    ML_CLASS_UNKNOWN     = 0,
    ML_CLASS_FINGERTAP   = 1,
    ML_CLASS_NORMAL      = 2,
    ML_CLASS_NOTCH_LONG  = 3,
    ML_CLASS_NOTCH_SHORT = 4,
} ML_CLASS_T;

void ML_APP_Init(void);

/* Call from the 20 kHz control ISR after the FOC step. Decimates to the
 * 2 kHz training sample rate and queues one sample for ML_APP_Task(). */
void ML_APP_Sample(void);

/* Call from the main loop. Feeds queued samples into the model and
 * prints a line on UART for every classification. */
void ML_APP_Task(void);

ML_CLASS_T  ML_APP_GetClass(void);
/* Duration of the inference that produced the last class, in us (0 = none yet). */
uint32_t    ML_APP_GetInferenceTimeUs(void);
const char *ML_APP_GetClassName(ML_CLASS_T cls);

#endif // ML_APP_H
