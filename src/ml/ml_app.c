#include "ml_app.h"
#include "mc/mc_app.h"
#include "kb.h"

#include <stdio.h>

/* Training data: X2Cscope prescaler 10 at the 20 kHz ISR -> 2 kHz */
#define ML_DECIMATION       10u

/* Sensor column order of the model (model.json sensors_columns) */
#define ML_NUM_SENSORS      3

/* Queue between ISR and main loop: 64 samples = 32 ms @ 2 kHz. Overflows
 * are counted in mlQueueOverflows (X2Cscope) if the main loop is too slow. */
#define ML_QUEUE_SIZE       64u

#define ML_MODEL_INDEX      KB_MODEL_SMART_MOTOR_PRED_MAINT_FOLD_0_INDEX

typedef struct {
    int16_t data[ML_NUM_SENSORS];  /* iq, vq, rpm */
} ml_sample_t;

static ml_sample_t queue[ML_QUEUE_SIZE];
static volatile uint16_t queueHead = 0;  /* written by ISR */
static volatile uint16_t queueTail = 0;  /* written by main loop */
static uint16_t decimationCount = 0;
static volatile bool motorRunning = false;
static bool modelArmed = false;

/* Visible in X2Cscope */
volatile uint8_t mlClass = ML_CLASS_UNKNOWN;
volatile uint32_t mlClassCount = 0;
volatile uint32_t mlQueueOverflows = 0;

static const char *const classNames[] = {
    "unknown", "fingertap", "normal", "notch_long", "notch_short"
};

void ML_APP_Init(void)
{
    kb_model_init();
    queueHead = 0;
    queueTail = 0;
    mlClass = ML_CLASS_UNKNOWN;
}

void ML_APP_Sample(void)
{
    MCAPP_CONTROL_SCHEME_T *pCS = &mcApp.controlScheme;

    /* The model only knows closed-loop operation at constant speed */
    motorRunning = (mcApp.appState == MCAPP_RUN) && (pCS->focState == FOC_CLOSE_LOOP);
    if (!motorRunning)
    {
        decimationCount = 0;
        return;
    }

    if (++decimationCount < ML_DECIMATION)
    {
        return;
    }
    decimationCount = 0;

    uint16_t next = (uint16_t)((queueHead + 1u) % ML_QUEUE_SIZE);
    if (next == queueTail)
    {
        mlQueueOverflows++;
        return;
    }

    /* Model Builder converted the float CSV values to int16 by truncation;
     * the same cast keeps the feature scaling of the Knowledge Pack valid. */
    queue[queueHead].data[0] = (int16_t)pCS->idq.q;
    queue[queueHead].data[1] = (int16_t)pCS->vdq.q;
    queue[queueHead].data[2] = (int16_t)pCS->estimatorInterface.speedMech.RPM;
    queueHead = next;
}

void ML_APP_Task(void)
{
    if (!motorRunning)
    {
        if (modelArmed)
        {
            /* Do not mix windows from before and after a stop */
            kb_flush_model_buffer(ML_MODEL_INDEX);
            modelArmed = false;
            mlClass = ML_CLASS_UNKNOWN;
        }
        queueTail = queueHead;
        return;
    }
    modelArmed = true;

    while (queueTail != queueHead)
    {
        int32_t ret = kb_run_model(queue[queueTail].data, ML_NUM_SENSORS, ML_MODEL_INDEX);
        queueTail = (uint16_t)((queueTail + 1u) % ML_QUEUE_SIZE);

        if (ret >= 0)
        {
            mlClass = (ret <= ML_CLASS_NOTCH_SHORT) ? (uint8_t)ret : ML_CLASS_UNKNOWN;
            mlClassCount++;
            printf("ML: %s\r\n", ML_APP_GetClassName((ML_CLASS_T)mlClass));
            kb_reset_model(ML_MODEL_INDEX);
        }
    }
}

ML_CLASS_T ML_APP_GetClass(void)
{
    return (ML_CLASS_T)mlClass;
}

const char *ML_APP_GetClassName(ML_CLASS_T cls)
{
    return (cls <= ML_CLASS_NOTCH_SHORT) ? classNames[cls] : classNames[ML_CLASS_UNKNOWN];
}
