#pragma once

#ifndef RTOS_TIME_H
#define RTOS_TIME_H

namespace RTOS {
namespace Time {

    /**
     * Initializes timer1 and configures it to interrupt every millisecond.
     */
    void init();

        // TODO manage current time manually
        // Could reimplment millis using a timer
        // milliseconds are probably as acccurate as we want to be
        // more accurate time requires more time spent updating the time
        // some fuzz helps guaruntee a correct schedule is maintained

        // Will probably have to add a configuration macro for what timer
        // should be used.

        // Eventually change to be a global value (register) ie.
        // i64 time = Registers::now;

        // We use a signed number so that math transformations of time will 
        // be safe

        // Could use micros() but then need to come up with a nice way to 
        // handle overflow

    /**
     * Returns the current time in ms.
     * 
     * @returns i64 the current time
     */
    i64 now();
    
    /**
     * Puts the processor into Idle Mode.
     * Will wake up if interrupted.
     */
    void idle_mode();

    /**
     * Idles for `time` from `from`. If time has already passed the idle time
     * will be reduced accordingly. Produces an idle trace.
     * 
     * @param i64 from the time to idle from
     * @param i64 time the amount of time to idle for
     */
    void idle(i64 from, i64 time);

}}

#endif /* RTOS_TIME_H */