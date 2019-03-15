#pragma once

#ifndef RTOS_PRIVATE_H
#define RTOS_PRIVATE_H

#ifdef RTOS_USE_ARDUINO
    void init_arduino();
#endif

namespace RTOS {

    namespace Registers {

        // A value that will be set with the bitwise ORed values of all active
        // events
        extern volatile Event_t events;

        // The last trace that occured.
        extern volatile Trace_t trace;

        // Memory pool used for the allocation of tasks
        extern Memory::Pool_t * task_pool;

        // Current task being run
        extern Task_t * current_task;

        // Head of a task list used for periodic tasks
        extern Task_t * periodic_tasks;

        // Head of a task list used for delayed tasks
        extern Task_t * delayed_tasks;

        // Head of a task list used for event tasks
        extern Task_t * event_tasks;

        // Tail of a task list used for event tasks
        extern Task_t * event_tasks_tail;

    }

    namespace Task {

        // Special case, task is an event task when it enters, but a periodic task when it leaves
        // Need to check if task has events on enter
        // If it does, need to manually remove it from event_tasks after
        // Not a big deal. Need to copy events anyways in order to clear the events register

        /**
         * Runs a task. Produces a trace for the start and stop of the task. If
         * the task is an event task, all events it responded to will be 
         * cleared.
         */
        void run(Task_t * task);

        /**
         * Returns true if the task is expected to fit in the given time 
         * window.
         * 
         * @param   Task_t * task the task
         * @returns bool          true if the task fits
         */
        bool fits(Task_t * task, i64 time);

        /**
         * Returns the next task in a task list. If the provided task list has
         * only a single item, nullptr is returned.
         * 
         * @param   Task_t * tasks the task list
         * @returns Task_t *       the next task in the list
         */
        Task_t * cdr(Task_t * tasks);

        /**
         * Inserts `task` as the tail of `tasks_tail`. 
         * 
         * @param   Task_t * tasks the tail of a task list
         * @param   Task_t * task  the task to insert
         * @returns Task_t *       the new tail of the task list
         */
        Task_t * insert_tail(Task_t * tasks_tail, Task_t * task);

        /**
         * Inserts `task` into `tasks` at an ordered position. Order is based 
         * which task should be scheduled next. If two tasks are scheduled for
         * the same time the task that wa salready in `tasks` should appear
         * first.
         * 
         * @param Task_t * tasks the head of a task list
         * @param Task_t * task  the task to insert
         * @returns Task_t *     the new head of the task list
         */
        Task_t * insert_ordered(Task_t * tasks, Task_t * task);

        /**
         * Calculates the next time the given task is expected to be run.
         * 
         * @param   Task_t * task the task
         * @returns i64           the next expected time
         */
        i64 time_next(Task_t * task);

        /**
         * Calculates the time remaining for a given task and a given point in
         * time. Time remaining is calulated by:
         * 
         *   (last_time + period + delay) - time
         * 
         * @param   Task_t * task the task
         * @param   i64      time the time to measure from
         * @returns i64           the remaining time
         */
        i64 time_remaining(Task_t * task, i64 time_ms);

    }

}

#endif /* RTOS_PRIVATE_H */