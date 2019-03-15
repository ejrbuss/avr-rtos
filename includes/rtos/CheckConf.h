#pragma once

#ifndef RTOS_CHECK_CONF_H
#define RTOS_CHECK_CONF_H

// 
// Checks RTOS Configuration
//

#if !defined(RTOS_VIRTUAL_HEAP) || RTOS_VIRTUAL_HEAP < 0 || RTOS_VIRTUAL_HEAP > 4096
#error RTOS Configuration Error: define RTOS_VIRTUAL_HEAP with a value between 0 and 4096
#include <FORCE STOP>
#endif

#if !defined(RTOS_MAX_EVENTS) || (RTOS_MAX_EVENTS != 8 && RTOS_MAX_EVENTS != 16 && RTOS_MAX_EVENTS != 32 && RTOS_MAX_EVENTS != 64)
#error RTOS Configuration Error: define RTOS_MAX_EVENTS as 8, 16, 32, or 64
#include <FORCE STOP>
#endif

#if !defined(RTOS_MAX_TASKS) || RTOS_MAX_TASKS < 0 || RTOS_MAX_TASKS > 64
#error RTOS Configuration Error: define RTOS_MAX_TASKS with a value between 0 and 64
#include <FORCE STOP>
#endif

#if !defined(RTOS_MESSAGE_BUFFER) || RTOS_MESSAGE_BUFFER < 0
#error RTOS Configuration Error: define RTOS_MESSAGE_BUFFER with a value greater than 0
#include <FORCE STOP>
#endif

#endif /* RTOS_CHECK_CONF_H */