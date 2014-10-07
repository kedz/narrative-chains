import time
import sys


class Timer(object):
    def __init__(self):
        self.start_ = None
        self.laps = []
        self.duration = 0
        self.stop_ = None

    def start(self, msg=None):
        if msg is not None:
            sys.stdout.write(msg)
            sys.stdout.flush()
        self.start_ = time.time()
        return self.start_

    def lap(self):
        
        self.laps.append(time.time())
        if len(self.laps) > 1:
            return self.laps[-1] - self.laps[-2]
        else:
            return self.laps[-1] - self.start_
    def stop(self):
        self.stop_ = time.time()
        self.duration = self.stop_ - self.start_
        return self.duration

    def elapsed(self):
        now = time.time()
        if self.start_ is not None:
            return now - self.start_
        else:
            return 0.0

    def reset(self):
        self.start_ = None
        self.laps = []
        self.duration = 0
        self.stop_ = None

def timed_function(func, args, msg):

    t = Timer()
    sys.stdout.write(msg)
    sys.stdout.flush()
    if not isinstance(args, list) and not isinstance(args, tuple):
        args = [args]
    t.start()
    result = func(*args)
    t.stop()
    sys.stdout.write('\t{}\n'.format(pretty_time(t.duration)))
    return result

def pretty_time(time_time_out):
    milliseconds = int(round(time_time_out * 1000))
    hours, milliseconds = divmod(milliseconds, 3600000)
    minutes, milliseconds = divmod(milliseconds, 60000)
    seconds, milliseconds = divmod(milliseconds, 1000)
    return '{:6.0f}h{:02.0f}m{:02.0f}s{:03.0f}ms'.format(
        hours, minutes, seconds, milliseconds)



