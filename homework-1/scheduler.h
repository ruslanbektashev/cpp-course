#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_NAME_MAX_LEN 32

typedef uint32_t task_id_t;

typedef struct TaskInfo {
    task_id_t id;
    char name[TASK_NAME_MAX_LEN];
    uint64_t period_ms;
    uint64_t next_run_ms;
} TaskInfo;

typedef struct Scheduler Scheduler;

Scheduler* scheduler_create(void);
void scheduler_destroy(Scheduler* s);

int scheduler_add_task(Scheduler* s, const char* name, uint64_t period_ms, uint64_t start_ms);
int scheduler_remove_task(Scheduler* s, task_id_t id);
int scheduler_get_task(const Scheduler* s, task_id_t id, TaskInfo* out);
size_t scheduler_task_count(const Scheduler* s);

void scheduler_update(Scheduler* s, uint64_t now_ms);
int scheduler_pop_ready(Scheduler* s, TaskInfo* out);
size_t scheduler_ready_count(const Scheduler* s);

#ifdef __cplusplus
}
#endif

#endif