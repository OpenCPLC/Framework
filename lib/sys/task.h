#ifndef TASK_H_
#define TASK_H_

#include "queue.h"
#include "vrts.h"


#ifndef TASK_LIMIT
  #define TASK_LIMIT 16
#endif

/**
 * @brief Task descriptor for time-scheduled execution.
 * @param[in] Handler Function to call when the task is executed.
 * @param[in] arg Opaque user pointer passed to `Handler`.
 * @param[in] tick Future-time marker.
 * @param[in] unique_key Optional stable identifier for uniqueness filtering, unused if `0`.
 */
typedef struct {
  void (*Handler)(void *);
  void *arg;
  uint64_t tick;
  int32_t unique_key;
} TASK_t;

void TASK_Add(void (*Handler)(void *), void *arg, uint32_t delay_ms);
void TASK_AddUnique(void (*Handler)(void *), void *arg, uint32_t delay_ms, int32_t unique_key);
void TASK_Main(void);

#endif