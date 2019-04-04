from scipy import signal
import numpy as np
import collections

#import matplotlib.pyplot as plt

def butter_bandpass(lowcut3dBHz, highcut3dBHz, fs, order=4):
    nyquist_freq = 0.5 * fs
    low = lowcut3dBHz / nyquist_freq
    high = highcut3dBHz / nyquist_freq
    b, a = signal.butter(order, [low, high], btype='band', analog=False)
    return b, a

def fir_bandpass(lowcut3dBHz, highcut3dBHz, fs, order=250):
    nyquist_freq = 0.5 * fs

    low = lowcut3dBHz / nyquist_freq
    high = highcut3dBHz / nyquist_freq

    beta = order/50.0
    taps = signal.firwin(order+1, [low, high], window=('kaiser', beta), pass_zero=False, scale=True)
    return taps

def butter_highpass(lowcut3dBHz, fs, order=4):
    nyquist_freq = 0.5 * fs
    low = lowcut3dBHz / nyquist_freq
    b, a = signal.butter(order, low, btype='highpass', analog=False)
    return b, a

def butter_lowpass(highcut3dBHz, fs, order=4):
    nyquist_freq = 0.5 * fs
    high = highcut3dBHz / nyquist_freq
    b, a = signal.butter(order, high, btype='lowpass', analog=False)
    return b, a


def butter_highpass_filter(values, lowcut3dBHz, fs, order=4):
    b, a = butter_highpass(lowcut3dBHz, fs, order=order)
    y = signal.lfilter(b, a, values)
    return y

class RealTimeFilterHighPassButter(object):

    def __init__(self, hp_3dBHz = 4, fs = 250,filter_order = 4):
        self.fs = fs
        self.filter_order = filter_order
        self.hp_3dBHz = hp_3dBHz
        if (self.hp_3dBHz >= 0) and (self.filter_order >= 0) and (self.hp_3dBHz is not None) and (self.filter_order is not None):
            b, a = butter_highpass(self.hp_3dBHz, self.fs, self.filter_order)
            self.a = a
            self.b = b
            self.zi = signal.lfiltic(self.b, self.a, [0])

    def fitlerNextSample(self,value):
        if (not (self.hp_3dBHz >= 0)) or (not (self.filter_order >= 0)) or (self.hp_3dBHz is None) or (self.filter_order is None):
            return value
        y, zf = signal.lfilter(self.b, self.a, [value], zi=self.zi)
        self.zi = zf
        return y[0]

class RealTimeFilterLowPassButter(object):

    def __init__(self, lp_3dBHz = 4, fs = 250, filter_order = 4):
        self.fs = fs
        self.filter_order = filter_order
        self.lp_3dBHz = lp_3dBHz
        if (self.lp_3dBHz >= 0) and (self.filter_order >= 0) and (self.lp_3dBHz is not None) and (self.filter_order is not None):
            b, a = butter_lowpass(self.lp_3dBHz, self.fs, self.filter_order)
            self.a = a
            self.b = b

            self.zi = signal.lfiltic(self.b, self.a, [0] * (max(len(self.a), len(self.b))-1))

    def fitlerNextSample(self, value):
        if (not (self.lp_3dBHz >= 0)) or (not (self.filter_order >= 0)) or (self.lp_3dBHz is None) or (self.filter_order is None):
            return value
        y, zf = signal.lfilter(self.b, self.a, [value], zi=self.zi)
        self.zi = zf
        return y[0]


class RealTimeFilterBandPassButter(object):

    def __init__(self, hp_3dBHz=8, lp_3dBHz=12, fs=250, filter_order=4):
        self.fs = fs
        self.filter_order = filter_order
        self.hp_3dBHz = hp_3dBHz
        self.lp_3dBHz = lp_3dBHz
        b, a = butter_bandpass(self.hp_3dBHz, self.lp_3dBHz, self.fs, self.filter_order)
        self.a = a
        self.b = b

        self.zi = signal.lfiltic(self.b, self.a, [0] * (max(len(self.a), len(self.b)) - 1))

    def fitlerNextSample(self, value):
        y, zf = signal.lfilter(self.b, self.a, [value], zi=self.zi)
        self.zi = zf
        return y[0]


class RealTimeFilterBandPassFIR(object):

    def __init__(self, hp_3dBHz=8, lp_3dBHz=12, fs=250, filter_order=101):
        self.fs = fs
        self.filter_order = filter_order
        self.hp_3dBHz = hp_3dBHz
        self.lp_3dBHz = lp_3dBHz
        self.taps = fir_bandpass(self.hp_3dBHz, self.lp_3dBHz, self.fs, self.filter_order)

        # self.buffer = collections.deque(maxlen=self.filter_order)
        # for i in range(0, self.filter_order):
        #     self.buffer.append(0)

        self.a = [1.0]
        self.b = self.taps

        self.zi = signal.lfiltic(self.b, self.a, [0] * (max(len(self.a), len(self.b)) - 1))


    def getDelayInSamples(self):
        return int(round(self.filter_order / 2.0))

    def fitlerNextSample(self, value):
        # self.buffer.append(value)
        # filtered_signal = signal.lfilter(self.taps, 1.0, value)
        # return filtered_signal[len(filtered_signal)]
        y, zf = signal.lfilter(self.b, self.a, [value], zi=self.zi)
        self.zi = zf
        return y[0]
