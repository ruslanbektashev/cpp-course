#include "task_scheduler.h"

#include <new>      /* std::nothrow */
#include <string.h> /* memset, strncpy */

/* Internal storage: no STL, no exceptions, deterministic order. */

struct TaskRecord {
    TsTaskInfo info;
    int        in_use; /* 0/1 */
};

struct ReadyQueue {
    TsTaskInfo* buf;
    size_t cap;
    size_t head;
    size_t tail;
    size_t size;
};

struct TaskScheduler {
    TaskRecord* tasks;
    size_t      tasks_cap;
    size_t      tasks_count;

    ReadyQueue  ready;
};

static void ts_copy_task(TsTaskInfo* dst, const TsTaskInfo* src) {
    memset(dst, 0, sizeof(*dst));
    dst->id = src->id;
    dst->period_ms = src->period_ms;
    dst->next_run_ms = src->next_run_ms;
    /* fixed-length name */
    if (src->name[0] != '\0') {
        strncpy(dst->name, src->name, TS_NAME_LEN - 1);
        dst->name[TS_NAME_LEN - 1] = '\0';
    }
}

static int rq_init(ReadyQueue* rq, size_t cap) {
    rq->buf = (TsTaskInfo*)::operator new(sizeof(TsTaskInfo) * cap, std::nothrow);
    if (!rq->buf && cap != 0) return 0;
    rq->cap = cap;
    rq->head = rq->tail = rq->size = 0;
    return 1;
}

static void rq_free(ReadyQueue* rq) {
    ::operator delete(rq->buf);
    rq->buf = 0;
    rq->cap = rq->head = rq->tail = rq->size = 0;
}

static int rq_push(ReadyQueue* rq, const TsTaskInfo* task) {
    if (!rq || rq->size >= rq->cap) return 0;
    rq->buf[rq->tail] = *task; /* POD copy */
    rq->tail = (rq->tail + 1) % rq->cap;
    rq->size++;
    return 1;
}

static int rq_pop(ReadyQueue* rq, TsTaskInfo* out) {
    if (!rq || rq->size == 0) return 0;
    *out = rq->buf[rq->head];
    rq->head = (rq->head + 1) % rq->cap;
    rq->size--;
    return 1;
}

static int find_task_index(const TaskScheduler* ts, uint32_t id, size_t* out_idx) {
    if (!ts) return 0;
    for (size_t i = 0; i < ts->tasks_cap; ++i) {
        if (ts->tasks[i].in_use && ts->tasks[i].info.id == id) {
            if (out_idx) *out_idx = i;
            return 1;
        }
    }
    return 0;
}

TaskScheduler* ts_create(size_t max_tasks, size_t ready_queue_capacity) {
    TaskScheduler* ts = new (std::nothrow) TaskScheduler;
    if (!ts) return 0;

    ts->tasks = 0;
    ts->tasks_cap = max_tasks;
    ts->tasks_count = 0;

    ts->tasks = (TaskRecord*)::operator new(sizeof(TaskRecord) * max_tasks, std::nothrow);
    if (!ts->tasks && max_tasks != 0) {
        delete ts;
        return 0;
    }

    for (size_t i = 0; i < max_tasks; ++i) {
        ts->tasks[i].in_use = 0;
        memset(&ts->tasks[i].info, 0, sizeof(TsTaskInfo));
    }

    if (!rq_init(&ts->ready, ready_queue_capacity)) {
        ::operator delete(ts->tasks);
        delete ts;
        return 0;
    }

    return ts;
}

void ts_destroy(TaskScheduler* ts) {
    if (!ts) return;
    rq_free(&ts->ready);
    ::operator delete(ts->tasks);
    delete ts;
}

int ts_add_task(TaskScheduler* ts, const TsTaskInfo* task) {
    if (!ts || !task) return TS_ERR_NULL;
    if (task->id == 0) return TS_ERR_BAD_ARG;

    /* enforce uniqueness */
    if (find_task_index(ts, task->id, 0)) return TS_ERR_DUPLICATE;

    /* find free slot (stable insertion order: first free) */
    for (size_t i = 0; i < ts->tasks_cap; ++i) {
        if (!ts->tasks[i].in_use) {
            ts->tasks[i].in_use = 1;
            ts_copy_task(&ts->tasks[i].info, task);
            ts->tasks_count++;
            return TS_OK;
        }
    }
    return TS_ERR_FULL;
}

int ts_remove_task(TaskScheduler* ts, uint32_t id) {
    if (!ts) return TS_ERR_NULL;
    size_t idx = 0;
    if (!find_task_index(ts, id, &idx)) return TS_ERR_NOT_FOUND;
    ts->tasks[idx].in_use = 0;
    memset(&ts->tasks[idx].info, 0, sizeof(TsTaskInfo));
    ts->tasks_count--;
    return TS_OK;
}

int ts_get_task(const TaskScheduler* ts, uint32_t id, TsTaskInfo* out_task) {
    if (!ts || !out_task) return TS_ERR_NULL;
    size_t idx = 0;
    if (!find_task_index(ts, id, &idx)) return TS_ERR_NOT_FOUND;
    *out_task = ts->tasks[idx].info;
    return TS_OK;
}

size_t ts_task_count(const TaskScheduler* ts) {
    return ts ? ts->tasks_count : 0;
}

int ts_update(TaskScheduler* ts, uint64_t now_ms) {
    if (!ts) return TS_ERR_NULL;

    /* scan in stable slot order */
    for (size_t i = 0; i < ts->tasks_cap; ++i) {
        if (!ts->tasks[i].in_use) continue;

        TsTaskInfo* t = &ts->tasks[i].info;

        if (t->next_run_ms > now_ms) continue;

        if (t->period_ms == 0) {
            /* one-shot: queue once and remove task */
            if (!rq_push(&ts->ready, t)) {
                /* if ready queue full, keep task; try next update */
                continue;
            }
            ts->tasks[i].in_use = 0;
            memset(&ts->tasks[i].info, 0, sizeof(TsTaskInfo));
            ts->tasks_count--;
        } else {
            /* periodic: queue all missed runs (deterministic), but limited by ready capacity */
            while (t->next_run_ms <= now_ms) {
                if (!rq_push(&ts->ready, t)) {
                    /* ready queue full -> stop; keep next_run_ms as-is for later */
                    break;
                }
                t->next_run_ms += (uint64_t)t->period_ms;
            }
        }
    }

    return TS_OK;
}

size_t ts_pop_ready(TaskScheduler* ts, TsTaskInfo* out_tasks, size_t max_tasks) {
    if (!ts || !out_tasks || max_tasks == 0) return 0;

    size_t n = 0;
    while (n < max_tasks) {
        if (!rq_pop(&ts->ready, &out_tasks[n])) break;
        n++;
    }
    return n;
}
