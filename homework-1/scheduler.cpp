#include "scheduler.h"
#include <map>
#include <queue>
#include <vector>
#include <algorithm>
#include <cstring>

class TaskSchedulerImpl {
public:
    TaskSchedulerImpl() : next_id_(1) {}
    
    ~TaskSchedulerImpl() {
        tasks_.clear();
        while (!ready_queue_.empty()) {
            ready_queue_.pop();
        }
    }
    
    task_id_t addTask(const char* name, uint64_t period_ms, uint64_t start_ms) {
        if (!name) {
            return 0;
        }
        
        TaskInfo task;
        task.id = next_id_++;
        
        std::strncpy(task.name, name, TASK_NAME_MAX_LEN - 1);
        task.name[TASK_NAME_MAX_LEN - 1] = '\0';
        
        task.period_ms = period_ms;
        task.next_run_ms = start_ms;
        
        tasks_[task.id] = task;
        
        return task.id;
    }
    
    bool removeTask(task_id_t id) {
        auto it = tasks_.find(id);
        if (it == tasks_.end()) {
            return false;
        }
        tasks_.erase(it);
        return true;
    }
    
    bool getTask(task_id_t id, TaskInfo* out) const {
        if (!out) {
            return false;
        }
        
        auto it = tasks_.find(id);
        if (it == tasks_.end()) {
            return false;
        }
        
        *out = it->second;
        return true;
    }
    
    size_t taskCount() const {
        return tasks_.size();
    }
    
    void update(uint64_t now_ms) {
        std::vector<TaskInfo> ready_tasks;
        std::vector<task_id_t> one_time_tasks_to_remove;
        
        for (auto& pair : tasks_) {
            TaskInfo& task = pair.second;
            
            if (task.next_run_ms <= now_ms) {
                ready_tasks.push_back(task);
                
                if (task.period_ms > 0) {
                    task.next_run_ms = now_ms + task.period_ms;
                } else {
                    one_time_tasks_to_remove.push_back(task.id);
                }
            }
        }
        
        std::sort(ready_tasks.begin(), ready_tasks.end(), 
                  [](const TaskInfo& a, const TaskInfo& b) {
                      return a.id < b.id;
                  });
        
        for (const auto& task : ready_tasks) {
            ready_queue_.push(task);
        }
        
        for (task_id_t id : one_time_tasks_to_remove) {
            tasks_.erase(id);
        }
    }
    
    bool popReady(TaskInfo* out) {
        if (!out || ready_queue_.empty()) {
            return false;
        }
        
        *out = ready_queue_.front();
        ready_queue_.pop();
        return true;
    }
    
    size_t readyCount() const {
        return ready_queue_.size();
    }

private:
    task_id_t next_id_;
    std::map<task_id_t, TaskInfo> tasks_;
    std::queue<TaskInfo> ready_queue_;
};

struct Scheduler {
    TaskSchedulerImpl* impl;
};

extern "C" {

Scheduler* scheduler_create(void) {
    try {
        Scheduler* scheduler = new Scheduler;
        scheduler->impl = new TaskSchedulerImpl();
        return scheduler;
    } catch (...) {
        return nullptr;
    }
}

void scheduler_destroy(Scheduler* s) {
    if (!s) return;
    
    delete s->impl;
    delete s;
}

int scheduler_add_task(Scheduler* s, const char* name, uint64_t period_ms, uint64_t start_ms) {
    if (!s || !s->impl || !name) {
        return -1;
    }
    
    task_id_t id = s->impl->addTask(name, period_ms, start_ms);
    if (id == 0) {
        return -1;
    }
    
    return static_cast<int>(id);
}

int scheduler_remove_task(Scheduler* s, task_id_t id) {
    if (!s || !s->impl) {
        return -1;
    }
    
    return s->impl->removeTask(id) ? 0 : -1;
}

int scheduler_get_task(const Scheduler* s, task_id_t id, TaskInfo* out) {
    if (!s || !s->impl || !out) {
        return -1;
    }
    
    return s->impl->getTask(id, out) ? 0 : -1;
}

size_t scheduler_task_count(const Scheduler* s) {
    if (!s || !s->impl) {
        return 0;
    }
    
    return s->impl->taskCount();
}

void scheduler_update(Scheduler* s, uint64_t now_ms) {
    if (!s || !s->impl) {
        return;
    }
    
    s->impl->update(now_ms);
}

int scheduler_pop_ready(Scheduler* s, TaskInfo* out) {
    if (!s || !s->impl || !out) {
        return 0;
    }
    
    return s->impl->popReady(out) ? 1 : 0;
}

size_t scheduler_ready_count(const Scheduler* s) {
    if (!s || !s->impl) {
        return 0;
    }
    
    return s->impl->readyCount();
}

}