#include <stdio.h>
#include "scheduler.h"

int main(void) {
    Scheduler* scheduler = scheduler_create();
    
    scheduler_add_task(scheduler, "Task A", 100, 0);   
    scheduler_add_task(scheduler, "Task B", 0, 50);    
    
    for (uint64_t time = 0; time <= 300; time += 50) {
        scheduler_update(scheduler, time);
        
        TaskInfo task;
        while (scheduler_pop_ready(scheduler, &task)) {
            printf("[%llu ms] %s\n", (unsigned long long)time, task.name);
        }
    }
    
    scheduler_destroy(scheduler);
    
    return 0;
}