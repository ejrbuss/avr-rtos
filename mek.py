from os.path import join

# This is a standard configuration file for mekpie

# the name of your project
name = 'avr-rtos'
# the c compiler configuration to use (gcc_clang, avr_gcc, or emscripten)
cc = avr_gcc(hardware='atmega2560', programmer='wiring', baud='115200')
# any libraries to load
libs = []
# additional compiler flags
flags = [
    '-Wall',
    # definitions
    '-DF_CPU=16000000L',
    '-DARDUINO=10808',
    '-DARDUINO_AVR_MEGA2560',
    '-DARDUINO_ARCH_AVR',
    # Need optimizations for delay to work
    '-Os',
    # Reduce application size and optimize
    '-flto',
    '-ffunction-sections',
    '-fdata-sections',
    '-fno-fat-lto-objects',
    '-fuse-linker-plugin',
    # Enable c++11 features
    '-std=c++11',
]

main = 'blink.cpp'

if options.release:
    flags += ['-O']
else:
    flags += ['-g']
