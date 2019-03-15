#include <RTOS.h>

#define LED_BIT 0b10000000
#define SERIAL_BAUD 115200

namespace RTOS {
namespace Trace {

    void configure_pin(Task_t * task, u8 pin) {
        #ifdef RTOS_USE_ARDUINO
            pinMode(pin, OUTPUT);
            task->impl.instance = pin | LED_BIT;
        #endif
    }

    void pin_trace(Trace_t * trace) {
        #ifdef RTOS_USE_ARDUINO
            switch (trace->tag) {
                case Mark_Start:
                    if (trace->mark.start.instance & LED_BIT) {
                        digitalWrite(trace->mark.start.instance & ~LED_BIT, HIGH);
                    }
                    break;
                case Mark_Stop:
                    if (trace->mark.stop.instance & LED_BIT) {
                        digitalWrite(trace->mark.stop.instance & ~LED_BIT, LOW);                        
                    }
                    break;
                default:
                    break;
            }
        #endif
    }

    void serial_trace(Trace_t * trace) {
        static bool first = true;
        #ifdef RTOS_USE_ARDUINO
            if (first) {
                Serial.begin(SERIAL_BAUD);
                Serial.write(sizeof(RTOS::Event_t));
                first = false;
            }
            u8 * trace_buffer = (u8 *) trace;
            for (u16 i = 0; i < sizeof(Trace_t); i++) {
                Serial.write(trace_buffer[i]);
            }
            if (trace->tag < Mark_Init || trace->tag == Debug_Message) {
                Serial.print(trace->def.handle);
                Serial.write('\0');
            }
        #endif
    }

}}