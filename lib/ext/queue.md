





```c
uint8_t *buff[128];
QUEUE_t queue;
QUEUE_Init(&queue, buff, sizeof(buff) / sizeof(uint32_t), sizeof(uint32_t), true);
queue.unique = false;

uint32_t task_push = 10;
QUEUE_Push(&queue, &x);
task_push = 20;
QUEUE_Push(&queue, &x);
task_push = 10; // elementy się powtózył wiec nie wejsdzie bo unniqe = true
QUEUE_Push(&queue, &x);
task_push = 5;
QUEUE_Push(&queue, &x);

uint32_t task_peek;
if(QUEUE_Peek(&queue, &task_peek)) {
  LOG_Info("Task (peek): %d", task_peek);
}

uint32_t task_pop;
while(!QUEUE_IsEmpty(&queue)) {
  QUEUE_Pop(&queue, &task_pop);
  LOG_Info("Task: %d", task_pop);
}
```



```c



````````

// <!-- typedef struct {
//   uint8_t id;
//   float value;
// } Sample_t;

// #define CAP 3
// Sample_t buf[CAP];

// QUEUE_t queue = {
//   .buffer = buf,
//   .elem_size = sizeof(Sample_t),
//   .capacity = CAP,
//   .unique = false,
//   .head = 0,
//   .tail = 0,
//   .count = 0
// };

// Sample_t s = { .id = 1, .value = 3.14f };
// QUEUE_Push(&queue, &s);

// Sample_t out;
// if(QUEUE_Pop(&queue, &out)) {
//   // out.id == 1, out.value == 3.14
// } -->