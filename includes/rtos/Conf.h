#pragma once

#ifndef RTOS_CONF
#define RTOS_CONF

//
// RTOS Configuration
//

// How much consumable memory the RTOS should provide
#define RTOS_VIRTUAL_HEAP 2048

// The maximum number of definable event. Must be 8, 16, 32, or 64.
#define RTOS_MAX_EVENTS 64

// The maximum number of tasks that can be instantiated at a given time.
// Maximum 64.
#define RTOS_MAX_TASKS 64

// Defining will cause RTOS to call RTOS::UDF::trace with trace info
#define RTOS_TRACE

// Checks may have performance overhead but help prevent undefined behaviour
#define RTOS_CHECK_ALL   // Enables all other checks
// #define RTOS_CHECK_ALLOC // Check bounds on allocation
// #define RTOS_CHECK_POOL  // Check bounds on pool allocation
// #define RTOS_CHECK_EVENT // Check bounds on event definitiom
// #define RTOS_CHECK_TASK  // Check bounds on task definition

// Max string buffer size for debug_print(...)
#define RTOS_MESSAGE_BUFFER 256

// Define if you want the RTOS to initialize the arduino library for you.
// Only REQUIRED if you use the builtin trace methods, `pin_trace` or 
//`serial_tracce`
#define RTOS_USE_ARDUINO

#endif /* RTOS_CONF */