from sys           import stderr
from struct        import unpack, calcsize
from mekpie.record import RecordClass

BYTE_ORDER   = '='
TAG_NAMES    = [
    'Def_Task',
    'Def_Event',
    'Def_Alloc',
    'Mark_Init',
    'Mark_Halt',
    'Mark_Start',
    'Mark_Stop',
    'Mark_Event',
    'Mark_Idle',
    'Mark_Wake',
    'Error_Max_Event',
    'Error_Undefined_Event',
    'Error_Max_Alloc',
    'Error_Max_Pool',
    'Error_Null_Pool',
    'Error_Max_Task',
    'Error_Null_Task',
    'Error_Invalid_Task',
    'Error_Duplicate_Event',
    'Error_Missed',
    'Debug_Message',
]
TAG_FIELDS = [
    ['handle', 'instance'], # Def_Task
    ['handle', 'event'],    # Def_Event
    ['handle', 'bytes'],    # Def_Alloc
    ['time', 'heap'],       # Mark_Init
    ['time'],               # Mark_Halt
    ['time', 'instance'],   # Mark_Start
    ['time', 'instance'],   # Mark_Stop
    ['time', 'event'],      # Mark_Event
    ['time'],               # Mark_Idle
    ['time'],               # Mark_Wake
    [],                     # Error_Max_Event
    ['event'],              # Error_Undefined_Event
    [],                     # Error_Max_Alloc
    [],                     # Error_Max_Pool
    [],                     # Error_Null_Pool
    [],                     # Error_Max_Task
    [],                     # Error_Null_Task
    [],                     # Error_Invalid_Task
    ['event'],              # Error_Duplicate_Event
    ['instance'],           # Error_Missed
    ['message'],             # Debug_Message
]

sizeof_trace = None 
tag_format   = None
formats      = None

def init_trace(tag_name, fields):
    field_names = ['name', 'tag'] + TAG_FIELDS[fields[0]]
    return RecordClass(dict(zip(field_names, [tag_name] + list(fields))))

def init_decoder(event_bytes):
    global sizeof_trace, tag_format, formats
    sizeof_event, = unpack('B', event_bytes)
    E = { 1: 'B', 2: 'H', 4: 'L', 8: 'Q'}[sizeof_event]
    sizeof_trace = max(
        calcsize(f'{BYTE_ORDER}HQ{E}'), 
        calcsize(f'{BYTE_ORDER}HQH'),
    ) 
    tag_format = f'{BYTE_ORDER}H'
    formats = [
        f'{BYTE_ORDER}HHB',   # Def_Task
        f'{BYTE_ORDER}HH{E}', # Def_Event
        f'{BYTE_ORDER}HHH',   # Def_Alloc
        f'{BYTE_ORDER}HQH',   # Mark_Init
        f'{BYTE_ORDER}HQ',    # Mark_Halt
        f'{BYTE_ORDER}HQB',   # Mark_Start
        f'{BYTE_ORDER}HQB',   # Mark_Stop
        f'{BYTE_ORDER}HQ{E}', # Mark_Event
        f'{BYTE_ORDER}HQ',    # Mark_Idle
        f'{BYTE_ORDER}HQ',    # Mark_Wake
        f'{BYTE_ORDER}H',     # Error_Max_Event
        f'{BYTE_ORDER}H{E}',  # Error_Undefined_Event
        f'{BYTE_ORDER}H',     # Error_Max_Alloc
        f'{BYTE_ORDER}H',     # Error_Max_Pool
        f'{BYTE_ORDER}H',     # Error_Null_Pool
        f'{BYTE_ORDER}H',     # Error_Max_Task
        f'{BYTE_ORDER}H',     # Error_Null_Task
        f'{BYTE_ORDER}HB',    # Error_Invalid_Task
        f'{BYTE_ORDER}H{E}',  # Error_Duplicate_Event
        f'{BYTE_ORDER}HB',    # Error_Missed
        f'{BYTE_ORDER}H',     # Debug_Message
    ]
    print(f'Initialized decoder - (sizeof trace: {sizeof_trace} sizeof event: {sizeof_event})', file=stderr)

def decode_cstring(serial):
    buffer = ''
    b = serial.read()
    while b != b'\x00':
        buffer += b.decode('ascii')
        b = serial.read()
    return buffer

def decode_trace(serial):
    if serial.in_waiting >= sizeof_trace:
        trace_bytes = serial.read(sizeof_trace)
        tag_bytes   = trace_bytes[:2]
        tag,        = unpack(tag_format, tag_bytes)
        tag_name    = TAG_NAMES[tag]
        byte_foramt = formats[tag]
        fields = unpack(byte_foramt, trace_bytes[:calcsize(byte_foramt)])
        # Check if we need to read the handler
        if tag_name.startswith('Def') or tag_name == 'Debug_Message':
            fields = (fields[0], decode_cstring(serial), *fields[2:])
        trace = init_trace(tag_name, fields)
        return trace