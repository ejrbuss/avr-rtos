#pragma once

#ifndef RTOS_TASK_H
#define RTOS_TASK_H

namespace RTOS {

    typedef struct Task_t Task_t;

    /**
     * The type of a task function. Task functions return a bool that indicates
     * whether they should be scheduled again, true they will be, false they
     * will be discarded and deallocated.
     * 
     * Task functions are passed a pointer to the task struct associated with
     * their call. They can use the `state` field to store relevant data and
     * modify `priod`, `delay`, and `events to determine their next schedule`.
     * 
     * eg.
     *   bool my_task_fn(Task_t * self) {
     *       ...
     *       return true; // Schedule me again please!
     *   }
     */
    typedef bool (*task_fn_t)(Task_t * self);

    /**
     * A Task defines the conditions and state for a task function run by the 
     * RTOS. Tasks can have a variety of behaviours:
     * 
     *  1. PERIODIC
     *     A task can be made to be run on a set period. The period is specified
     *     using the `period` field. The task can be offset by some amount of 
     *     time in the overall schedule by delaying its first run. A Task's
     *     delay is always returned to zero once the delay has occured. A Task
     *     with a period of 0 will not be scheduled run more than once unless it
     *     either has a delay greater than 0 or its `events` field is set to 
     *     respond to some event. Additionally, a task with a period of 0 will
     *     have a lower priority than a task with a non 0 priortiy. In cases 
     *     where tasks conflic, tasks will be scheduled on a first in first out
     *     basis.
     * 
     * eg.
     *   use RTOS;
     * 
     *   Task_t * my_task = Task::init("my_task", my_task_fn);
     *   my_task->period = 1000;  // Schedule my task once a second
     *   my_task->delay  = 500;   // Offset my task by half a second
     *   Task::dispatch(my_task); // Indicate my task is ready
     * 
     *  2. DELAYED
     *     A task can be made to run once after a set delay. The task will then
     *     stop running unless the delay is again set to some value greater 
     *     than 0 and the task function returns true.
     * 
     * eg.
     *   use RTOS;
     * 
     *   Task_t * my_task = Task::init("my_task", my_task_fn);
     *   my_task->delay = 1000; // Run my task in 1 second
     *   Task::dispatch(my_task);
     * 
     *  3. EVENT DRIVEN
     *     A task can be made to run after a specified event occurs. These 
     *     tasks will be run during RTOS idle time. A task can be made to run
     *     after multiple events, however multiple tasks cannot listen for the
     *     same event. Event driven task should NOT have a period greater than 
     *     0. 
     * 
     *  eg. 
     *    use RTOS;
     *    
     *    // Create some events to wait for
     *    Event_t EVENT_1 = Event::init("EVENT_1");
     *    Event_t EVENT_2 = Event::init("EVENT_2");
     *    
     *    Task_t * my_task = Task::init("my_task", my_task_fn);
     *    my_task->events |= EVENT_1; // Run after EVENT_1 is dispatched
     *    my_task->events |= EVENT_2; // Run after EVENT_2 is dispatched
     *    Task::dispatch(my_task);
     *
     *     The preffered method to trigger multiple tasks from a single event
     *     is to use delayed tasks. This can be preferrable to a single task
     *     doing all the work as it allows the tasks to be performed during
     *     different idle times. Note that the tasks will be performed in a 
     *     first in first out order.
     * eg. 
     *   bool multi_task_fn() {
     *       Task::dispatch(Task::init("my_task_1", my_task_1_fn));
     *       Task::dispatch(Task::init("my_task_2", my_task_2_fn));
     *       Task::dispatch(Task::init("my_task_3", my_task_3_fn));
     *       return true;
     *   }
     * 
     *  4. IMMEDIATE
     *     A task will be scheduled immediately if it has a period of 0, delay
     *     of 0, and responds to no events. If there are periodic tasks 
     *     waiting, they will still run first. As stated previously such a task 
     *     will only be run once unless `period`, `delay`, or `events` are 
     *     modified during execution. 
     * 
     * eg.
     *   use RTOS;
     * 
     *   Task::dispatch(Task::init("my_task", my_task_fn));
     * 
     */
    struct Task_t {
        task_fn_t fn;   // A pointer to the task funtion
        void * state;   // A pointer to the task's associated state
        Event_t events; // The events that cause this task to be scheduled
        i16 period_ms;  // The schedule period of this task (in milliseconds)
        i16 delay_ms;   // The delay before this task is scheduled
        // "hidden" fields
        struct {
            bool first;
            u8 instance; // Used to identify a task during a trace
            i64 last;    // The last time this task was run
            i16 maximum; // The maximum runtime of this task so far
        } impl;
    };

    namespace Task {

        /**
         * Allocates a new Task. `fn` will be called if the task is scheduled
         * and passed this task. The caller is expected to configure the fields
         * of the task after calling this function, but before callig dispatch. 
         * By default a task has a NULL state, is scheduled for no events, and
         * has a period and delay of 0.
         * 
         * Tasks are allocated using a pool. If the number of tasks exceeds 
         * RTOS_MAX_TASKS and RTOS_CHECK_TASK is defined this function will 
         * produce an error trace, otherwise this function is considered 
         * undefined at this point.
         * 
         * If RTOS_TRACE is defined this will produce an init trace.
         * 
         * @param   const char * handle the debugging hangle
         * @param   task_fn_t    fn     the task function
         * @returns Task_t *            a pointer to task
         */
        Task_t * init(const char * handle, task_fn_t fn);

        /**
         * Dipsatches the task to the scheduler. If RTOS_CHECK_TASK is defined
         * the fields of this task wil lbe verified such that if `period` or 
         * `delay` is set than `events` will be 0, or vice versa. Will also 
         * check that no other task is set to the same events.
         * 
         * @param Task_t * task the tast to dispatcj
         */
        void dispatch(Task_t * task);

    }

}

#endif /* RTOS_TASK_H */