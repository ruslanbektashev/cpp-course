#include <stdio.h>
#include <string.h>
#include "task_scheduler.h"

static void print_task(const TsTaskInfo* t) {
    printf("Task{id=%u, name='%s', period=%u ms, next=%llu}\n",
           t->id, t->name, t->period_ms, (unsigned long long)t->next_run_ms);
}

int main(void) {
    TaskScheduler* ts = ts_create(16, 64);
    if (!ts) {
        printf("Failed to create scheduler\n");
        return 1;
    }

    TsTaskInfo t1;
    memset(&t1, 0, sizeof(t1));
    t1.id = 1;
    strncpy(t1.name, "blink", TS_NAME_LEN - 1);
    t1.period_ms = 100;
    t1.next_run_ms = 100;

    TsTaskInfo t2;
    memset(&t2, 0, sizeof(t2));
    t2.id = 2;
    strncpy(t2.name, "oneshot", TS_NAME_LEN - 1);
    t2.period_ms = 0;
    t2.next_run_ms = 250;

    ts_add_task(ts, &t1);
    ts_add_task(ts, &t2);

    for (unsigned now = 0; now <= 500; now += 50) {
        ts_update(ts, now);

        TsTaskInfo ready[8];
        size_t n = ts_pop_ready(ts, ready, 8);

        if (n) {
            printf("\nnow=%u ms -> ready=%zu\n", now, n);
            for (size_t i = 0; i < n; ++i) {
                print_task(&ready[i]);
                /* Here external code would EXECUTE the task */
            }
        }
    }

    ts_destroy(ts);
    return 0;
}
