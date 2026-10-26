#include "ml_app.h"
#include "mc/mc_app.h"
#include "kb.h"
#include "mcc_generated_files/timer/sccp2.h"

#include <stdio.h>

/* AI_Timer (SCCP2): free-running 32-bit, Clock Generator 13 = 200 MHz, 1:1,
 * period 50 ms (10,000,000 ticks). One tick = 5 ns. */
#define AI_TIMER_NS_PER_TICK    5u
#define AI_TIMER_PERIOD_TICKS   (AI_Timer_PeriodGet() + 1u)

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
/* Duration of the kb_run_model() call that produced the last classification,
 * and the longest such call since boot, in microseconds. */
volatile uint32_t mlInferenceTimeUs = 0;
volatile uint32_t mlInferenceTimeMaxUs = 0;

static uint32_t ai_timer_elapsed_us(uint32_t start, uint32_t end)
{
    /* The counter wraps at the period match; one wrap at most per call. */
    uint32_t ticks = (end >= start) ? (end - start) : (end + AI_TIMER_PERIOD_TICKS - start);
    return (ticks * AI_TIMER_NS_PER_TICK + 500u) / 1000u;
}

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
        uint32_t t0 = AI_Timer_CounterGet();
        int32_t ret = kb_run_model(queue[queueTail].data, ML_NUM_SENSORS, ML_MODEL_INDEX);
        uint32_t t1 = AI_Timer_CounterGet();
        queueTail = (uint16_t)((queueTail + 1u) % ML_QUEUE_SIZE);

        if (ret >= 0)
        {
            /* Only the call that completes a window runs feature extraction
             * and the classifier; the others just buffer a sample. */
            mlInferenceTimeUs = ai_timer_elapsed_us(t0, t1);
            if (mlInferenceTimeUs > mlInferenceTimeMaxUs)
            {
                mlInferenceTimeMaxUs = mlInferenceTimeUs;
            }
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

uint32_t ML_APP_GetInferenceTimeUs(void)
{
    return mlInferenceTimeUs;
}

const char *ML_APP_GetClassName(ML_CLASS_T cls)
{
    return (cls <= ML_CLASS_NOTCH_SHORT) ? classNames[cls] : classNames[ML_CLASS_UNKNOWN];
}
