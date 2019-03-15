/**
 * blink
 * 
 * Test: 
 *  A single periodic task that blinks an LED forever.
 */

#include <RTOS.h>

bool led = false;

bool task_led_fn(RTOS::Task_t * task) {
    led = !led;
    digitalWrite(LED_BUILTIN, led);
    return true;
}

int main() {
    RTOS::init();
    pinMode(LED_BUILTIN, OUTPUT);
    RTOS::Task_t * task_led = RTOS::Task::init("task_led", task_led_fn);
    task_led->period_ms = 500;
    RTOS::Task::dispatch(task_led);
    RTOS::dispatch();
    return 0;
}

namespace RTOS {
namespace UDF {
    void trace(Trace_t * trace) {
        Trace::serial_trace(trace);
    }
    bool error(Trace_t * trace) { return true; }
}}