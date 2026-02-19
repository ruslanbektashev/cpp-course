#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TS_NAME_LEN 32

typedef struct TaskScheduler TaskScheduler;

/* Public, C-friendly snapshot of a task. */
typedef struct TsTaskInfo {
    uint32_t id;
    char     name[TS_NAME_LEN];
    uint32_t period_ms;     /* 0 = one-shot */
    uint64_t next_run_ms;   /* absolute time in ms */
} TsTaskInfo;

/* Return codes (0 = success, negative = error) */
enum {
    TS_OK            = 0,
    TS_ERR_NULL      = -1,
    TS_ERR_FULL      = -2,
    TS_ERR_DUPLICATE = -3,
    TS_ERR_NOT_FOUND = -4,
    TS_ERR_BAD_ARG   = -5
};

/* Lifecycle */
TaskScheduler* ts_create(size_t max_tasks, size_t ready_queue_capacity);
void ts_destroy(TaskScheduler* ts);

/* Task management */
int    ts_add_task(TaskScheduler* ts, const TsTaskInfo* task);
int    ts_remove_task(TaskScheduler* ts, uint32_t id);
int    ts_get_task(const TaskScheduler* ts, uint32_t id, TsTaskInfo* out_task);
size_t ts_task_count(const TaskScheduler* ts);

/*
 * Time update:
 * - scans tasks in stable insertion order,
 * - moves due tasks into internal "ready" queue,
 * - reschedules periodic tasks,
 * - removes one-shot tasks once they are queued as ready.
 */
int ts_update(TaskScheduler* ts, uint64_t now_ms);

/*
 * Pop ready tasks into user buffer (FIFO).
 * Returns number of tasks written to out_tasks.
 */
size_t ts_pop_ready(TaskScheduler* ts, TsTaskInfo* out_tasks, size_t max_tasks);

#ifdef __cplusplus
}
#endif

#endif /* TASK_SCHEDULER_H */
