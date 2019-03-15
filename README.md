# avr-rtos

## What's in this Repository
This repository includes all of the source code for our AVR RTOS. You can find the source files in `/src` and the headers in `/includes`. Additionally an example blink program is included which should flash the LED light every 1000 ms. 

In addition to the RTOS source a modified copy of the Arduino core libraries are included. This allows you to use any arduino functions you may have grown used to in the Arduino IDE. These sources have been modified: 

1. We removed the arduino main, you must define your own main function and initizliing components appropriately

2. We stole some timers from Arduino, namely timer 1 and timer 3, we use timer 1 to keep time with our RTOS and timer 3 is reserved for you to use for additional timing needs.

If you DO NOT want Arduino files in your project make sure you change `/includes/rtos/Conf.h` and comment out `#define RTOS_USE_ARDUINO`.

## How do I Build this?
The whole project is just a bunch of C++ files and headers so you should just be able to copy and paste it all into your project in Atmel Studio or Eclipse. We built the RTOS using my tool however [mekpie](https://ejrbuss.net/mekpie/). Its a command line tool for building c projects. To use it you need to:

1. Install it (`pip install mekpie`)
2. Have `avr-gcc` and `avrdude` on your path and yse the arduino `avrdude` configuration file as your default configuration. You can do this if you copy the `avrdude.conf` file from `Program Files (x86)/Arduino/hardware/tools/avr/etc` into `Program Files (x86)/Arduino/hardware/tools/avr/bin` and then add `Arduino/hardware/tools/avr/bin` to your path. This is only the case for windows. For Mac you can just use [homebrew](https://brew.sh/) and `brew install avr-gcc`.

Once setup if you open a terminal to this repo and run `mekpie run` your project should build and you'll be asked to enter a COM port for uploading. You can use Arduino IDE to figure out what COM port the board is using.

If you want to use the serial tracing just run `mekpie run && python3 -m tracer`, this will show a JSON log of the trace for the default blink.cpp program and if you open the provided webpage (shown in the terminal output, but it should be http://localhost:3000) you can see a live trace of your tasks.

## What's this Tracing Stuff
The RTOS is instrumented to make it easy to debug and examine the schedule. You can look at the code examples and headers to get a sense of how this works, but basically we pass a struct to a user callback with useful information anytime something important happens in the RTOS.

## Where's the Documentation
Most of our documentation is found in the rtos header files (`/includes/rtos/...`). But here are the basics.

```c
// Task functions should have the following signature
bool my_task_function(RTOS::Task_t * self) {
    // You can find out the time with
    i64 the_time = RTOS::Time::now();
    // Return true if you want the tast to run again
    return true;
}

int main() {
    // Initialize the RTOS (do this first)
    // this will also initialize Arduino if you include it
    RTOS::init();

    // Create a Task
    RTOS::Task_t task = RTOS::Task::init("task name", my_task_function);

    // You can set the period like so
    task->period_ms = 1000;

    // You can set the delay as well
    task->delay_ms = 500;

    // You can associate a task with a digital pin 
    // (this will break serial tracing, you can only use one or the other)
    RTOS::Trace::configure_pin(task, 3);

    // Create an Event
    RTOS::Event_t event = RTOS::Event::init("event name");

    // Have a task listen for an event
    task->events |= event;

    // Have an event go off (you would do this in an ISR or a task function)
    RTOS::Event::dispatch(event);

    // When the task is good to go you must dispatch it
    RTOS::Task::dispatch(task);

    // When all of your tasks are created and dispatched tell the RTOS 
    // to take it away. This function woill not return until you call
    // RTOS::halt from one of your task, or an error occurs.
    RTOS::dispatch(); 
}

// You need to define two User Defined Functions (UDF)
namespace RTOS {
namespace UDF {

    // This function is called with tracing information
    void trace(Trace_t * trace) {
        // You can output it on Serial (USB)
        RTOS::Trace::serial_trace(trace);
        // You can output it to digital pins (Logic Analyzer)
        RTOS::Trace::pin_trace(trace);
        // Or do your own thing
        // Or do nothing
    }

    // This function is called when an error occurs 
    // (for instance a task missees itss scheduled time)
    bool error(Trace_t * trace) {
        // Return true if you want the RTOS to keep on trudging
        return true;
    }

}}
```

**Important:** If you want more examples on how to do things, check out our test cases listed at the bottom of this [page](https://sites.google.com/view/csc-460/project-2?authuser=0) (links to source code is there as well).