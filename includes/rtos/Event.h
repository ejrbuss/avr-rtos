#pragma once

#ifndef RTOS_EVENT_H
#define RTOS_EVENT_H

namespace RTOS {

    /**
     * A type representing an event. Events are encoded in a single integer, 
     * withc each bit signifying a different event. The size of integer used 
     * for Event_t determines the number of possible events that can be
     * represented.
     */
    typedef NUMTYPE_U(RTOS_MAX_EVENTS) Event_t;

    namespace Event {

        /**
         * Creates a new Event_t with the corresponding handle. Events are just
         * a numeric value. Produces a trace that defines the event. If 
         * RTOS_MAX_EVENTS is exceeded and RTOS_CHECK_EVENT is defined than a 
         * trace error is produced, otherwise this functions behaviour is 
         * undefined at this point.
         * 
         * eg.
         *   use RTOS;
         * 
         *   const Event_t MY_EVENT = Event::with_handle("my_event");
         * 
         * @param   const char * handle the debug handle
         * @returns Event_t             the event number 
         */
        Event_t init(const char * handle);

        /**
         * Dispatches an event. Any task waiting for this event will be 
         * scheduled in the next idle period. if RTOS_CHECK_EVENT is defined
         * and the provided event was never created using Event::init a trace
         * error will be produced.
         * 
         * @param Event_t e the event to schedule
         */
        void dispatch(Event_t e);

    }

}

#endif /* RTOS_EVENT_H */