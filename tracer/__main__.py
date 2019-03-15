from argparse import ArgumentParser

from .tracer import main

# Program entry point for module
if __name__ == "__main__":
    parser = ArgumentParser(prog='tracer', description='RTOS live tracer provides debug information from a serial connection to your AVR board.')
    parser.add_argument('--noweb', '-n', action='store_true', help='run program without web server')
    parser.add_argument('--debug', '-d', action='store_true', help='run program in debug mode')
    parser.add_argument('--port',  '-p', default=3000, type=int, nargs=1, help='web server port number (default 3000)')
    parser.add_argument('--max',   '-m', default=256,  type=int, nargs=1, help='maximum number of traces to log (default 256)')
    main(parser.parse_args())