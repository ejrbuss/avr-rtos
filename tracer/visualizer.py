from mekpie.record        import RecordClass
from matplotlib           import pyplot
from matplotlib.animation import FuncAnimation
from matplotlib.gridspec  import GridSpec
from matplotlib.widgets   import SpanSelector, Button
from matplotlib.patches   import Rectangle

import numpy as np

# The last 5 seconds
REALTIME_WINDOW = 5000

def init_memory_map(ax):
    ax.plot([], [])
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.get_xaxis().set_visible(False)
    ax.get_yaxis().set_visible(False)

    r = Rectangle((0, 0), 0.5, 0.5, facecolor='red')

    ax.add_patch(r)

def init_realtime_plot(ax):
    ax.get_xaxis().set_visible(False)

def init_trace_full(ax):
    ax.get_xaxis().set_visible(False)

def init_trace_zoom(ax):
    ax.get_xaxis().set_visible(False)

def init_system_table(ax):
    ax.plot([], [])
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.get_xaxis().set_visible(False)
    ax.get_yaxis().set_visible(False)

def visualize(update, *fargs):

    state = RecordClass({
        'tasks'         : [],
        'tasks_lookup'  : [],
        'last_time'     : 0,
        'time'          : 0,
        'stop'          : False,
        'update'        : False,
        'refresh'       : False,
        'refresh_time'  : 0,
    })

    def stop(event):
        state.stop = True

    def refresh(event):
        state.refresh_time = state.time
        state.refresh = True

    fig = pyplot.figure()
    gs  = GridSpec(3, 2, width_ratios=[1, 3])

    memory_map    = fig.add_subplot(gs[:2, 0])
    realtime_plot = fig.add_subplot(gs[0,  1])

    refresh_ax = pyplot.axes([0.125, 0.9, 0.1, 0.06])
    refresh_button = Button(refresh_ax, 'Refresh')
    refresh_button.color = '#FFFFFF'
    refresh_button.on_clicked(refresh)

    stop_ax = pyplot.axes([0.235, 0.9, 0.1, 0.06])
    stop_button = Button(stop_ax, 'Stop')
    stop_button.color = '#FFFFFF'
    stop_button.on_clicked(stop)

    trace_full   = fig.add_subplot(gs[1, 1])
    system_table = fig.add_subplot(gs[2, 0])
    trace_zoom   = fig.add_subplot(gs[2, 1])

    init_memory_map(memory_map)
    init_realtime_plot(realtime_plot)
    init_trace_full(trace_full)
    init_trace_zoom(trace_zoom)
    init_system_table(system_table)

    def animate(frame):
        
        # Do not update if we are stopped
        if state.stop:
            return

        update(frame, state, *fargs) 

        # Do not update if nothing has changed        
        if not state.update and state.time == state.last_time:
            return
        
        state.last_time = state.time

        yticks = [n * 1.5 + 0.5 for n in range(len(state.tasks_lookup))]
        
        realtime_plot.clear()
        realtime_plot.set_yticks(yticks)
        realtime_plot.set_yticklabels(state.tasks_lookup)
        realtime_plot.set_xlim(
            max(state.last_time - REALTIME_WINDOW, 0),
            max(state.last_time, REALTIME_WINDOW) + REALTIME_WINDOW / 10,
        )
        for task in state.tasks:
            x, y = task
            realtime_plot.plot(
                x[-100:] + [state.last_time],
                y[-100:] + [y[-1]],
                'k',
                linewidth=0.5,
            )
        if state.refresh:
            state.refresh = False

            trace_full.clear()
            trace_zoom.clear()
            trace_full.set_yticks(yticks)
            trace_zoom.set_yticks(yticks)
            trace_full.set_yticklabels(state.tasks_lookup)
            trace_zoom.set_yticklabels(state.tasks_lookup)
            trace_full.set_xlim(
                0,
                max(state.last_time, REALTIME_WINDOW),
            )
            for task in state.tasks:
                x, y = task
                imin, imax = np.searchsorted(x, (0, state.refresh_time))
                trace_full.plot(
                    x[imin:imax] + [state.last_time],
                    y[imin:imax] + [y[-1]],
                    'k',
                    linewidth=0.5,
                )

            return [realtime_plot, trace_full, trace_zoom]
        if state.update:
            state.update = False
            return [realtime_plot, ]
        else:
            return realtime_plot

    def onselect(xmin, xmax):

        # yticks = [n * 1.5 + 0.5 for n in range(len(state.tasks_lookup))]
        
        trace_zoom.clear()
        trace_zoom.set_xlim(xmin, xmax)
        for task in state.tasks:
            x, y = task
            imin, imax = np.searchsorted(x, (xmin / 2, xmax + xmin / 2))
            trace_zoom.plot(
                x[imin:imax] + [state.last_time],
                y[imin:imax] + [y[-1]],
                'k',
                linewidth=0.5,
            )

    span = SpanSelector(trace_full, onselect, 'horizontal',
        useblit=True,
        span_stays=True,
        rectprops=dict(alpha=0.5, facecolor='red')
    )
    ani  = FuncAnimation(fig, animate),
    pyplot.show()