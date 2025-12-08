#include "task.h"
#include "log.h"

static bool TASK_Equal(const void *a, const void *b)
{
  const TASK_t *task_a = a, *task_b = b;
  return task_a->unique_key && task_b->unique_key && (task_a->unique_key == task_b->unique_key);
}

static int32_t TASK_Compare(const void *a, const void *b)
{
  const TASK_t *task_a = a, *task_b = b;
  if(task_a->tick < task_b->tick) return -1;
  if(task_a->tick > task_b->tick) return 1;
  return 0;
}

static TASK_t task_buffer[TASK_LIMIT];
static QUEUE_t task_queue = {
  .buffer = (void *)task_buffer,
  .struct_size = sizeof(TASK_t),
  .capacity = TASK_LIMIT,
  .unique = true,
  .invert = true,
  .Equal = TASK_Equal,
  .Compare = TASK_Compare
};

static inline void TASK_FullError(void)
{
  LOG_Error("Task not added: the queue is full" LOG_LIB("TASK"));
}

/**
 * @brief Schedule a task after a delay.
 * @param[in] Handler Function to run with delay.
 * @param[in] arg User data passed to Handler.
 * @param[in] delay_ms Delay in milliseconds.
 */
void TASK_Add(void (*Handler)(void *), void *arg, uint32_t delay_ms)
{
  if(!delay_ms) {
    Handler(arg);
    return;
  }
  TASK_t task = {
    .Handler = Handler,
    .arg = arg,
    .tick = tick_keep(delay_ms),
    .unique_key = 0
  };
  bool ok = QUEUE_Push(&task_queue, &task);
  if(!ok) TASK_FullError();
}

/**
 * @brief Schedule a task with a uniqueness key after a delay.
 * @param[in] Handler Function to run with delay.
 * @param[in] arg User data passed to Handler.
 * @param[in] delay_ms Delay in milliseconds.
 * @param[in] unique_key Application-defined key for de-duplication. Unused if `0`.
 */
void TASK_AddUnique(void (*Handler)(void *), void *arg, uint32_t delay_ms, int32_t unique_key)
{
  TASK_t task = {
    .Handler = Handler,
    .arg = arg,
    .tick = tick_keep(delay_ms),
    .unique_key = unique_key
  };
  bool ok = QUEUE_Push(&task_queue, &task);
  if(!ok) TASK_FullError();
}

/**
 * @brief Task scheduler loop.
 * Runs forever: executes due tasks and never returns.
 */
void TASK_Main(void)
{
  TASK_t task;
  while(1) {
    if(QUEUE_Peek(&task_queue, &task)) {
      if(tick_over(&task.tick)) {
        task.Handler(task.arg);
        QUEUE_Pop(&task_queue, &task);
      }
    }
    let();
  }
}