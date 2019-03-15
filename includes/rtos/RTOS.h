#pragma once

#ifndef RTOS_H
#define RTOS_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

//
// Macros
//

// Macros for generating numeric types
#define NUMTYPE_U_IMPL(n) u ## n
#define NUMTYPE_U(n) NUMTYPE_U_IMPL(n)
#define NUMTYPE_I_IMPL(n) i ## n
#define NUMTYPE_I(n) NUMTYPE_i_IMPL(n)
#define BV(bit) (1 << (bit))

//
// Exact numeric types
//

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned long      u32;
typedef unsigned long long u64;

typedef signed char      i8;
typedef signed int       i16;
typedef signed long      i32;
typedef signed long long i64;

#include "Conf.h"
#include "CheckConf.h"

#ifdef RTOS_USE_ARDUINO
    #include "Arduino.h"
#endif

#include "Event.h"
#include "Time.h"
#include "Memory.h"
#include "Task.h"
#include "Trace.h"

namespace RTOS {

    /**
     * Initializes the RTOS.
     */
    void init();
    
    /**
     * Starts the RTOS.
     */
    void dispatch();

    /**
     * Immediately halts the execution of the RTOS, producing a halt trace.
     */
    void halt();

    /**
     * Traces the current trace register. MUST BE CALLED IN AN ATOMIC BLOCK!
     */
    void trace();

    /**
     * Traces the current trace register sending it to both UDF::trace and 
     * UDF::error.
     */
    void error();

    /**
     * Creates a message trace with the provided format (ala printf).
     * 
     * eg.
     * 
     *    debug_print("(%s: %d)", name, value);
     * 
     * @param char * fmt the string format
     * @param ...        format args
     */ 
    void debug_print(const char * fmt, ...);

    /**
     * Sets the builtin LED on or off for debugging purposes.
     * 
     * If RTOS_USE_ARDUINO is not defined this function does nothing.
     * 
     * @param bool led true for on, false for odd
     */
    void debug_led(bool led);

    /**
     * Registers are bits of data the RTOS needs to share across its entire
     * system and may want to share publically.
     */
    namespace Registers {

        // An event register used to store the triggering events
        extern Event_t triggers;

    }

    /**
     * User defined functions, these must be implemented by YOU!
     */
    namespace UDF {
        
        /**
         * Handles tracing data. Traces are provided for a variety actions that
         * occur in the RTOS (see Trace.h for more information). This function
         * is only called if RTOS_TRACE is defined. 
         * 
         * RTOS provides two builtin methods of handling trace data:
         * 
         *  1. PIN MODE
         *     In PIN MODE RTOS will set various available digital pins high
         *     and low to indicate when tasks are running.
         *  2. SERIAL MODE
         *     In SERIAL MODE RTOS will send all trace informatin out on a 
         *     serial port as it comes in.
         * 
         * eg.
         * 
         *    int main() {
         *       RTOS::init();
         *       ...
         *       // Set the pin for a given task
         *       RTOS::Trace::PinTrace(my_task, PIN_12);
         *       RTOS::dispatch();
         *       return 0;
         *   }
         *  
         *   namespace RTOS::UDF {
         *       void trace(Trace_t * trace) {
         *           // Tell PIN MODE to handle trace
         *           RTOS::Trace::PinMode(trace);
         *       }
         *   }
         * 
         * See Trace.h for how to use these builtin modes
         * 
         * @param const Trace_t * trace the trace action that just occured
         */
        void trace(Trace_t * trace);

        /**
         * This function is called with an error trace regardless if RTOS_TRACE
         * is defined. This function provides you a chance to handle errors 
         * gracefully. Returning true from this function will resume scheduling.
         * Returning false from this function will halt scheduling.
         */
        bool error(Trace_t * trace);

    }
    
}

#endif /* RTOS_H */