import sys
import time
import timeit
import csv
from open_bci_v3 import OpenBCISample
from collections import namedtuple
# # from PySide import QtGui, QtCore  # (the example applies equally well to PySide)
# from PyQt4 import QtGui, QtCore  # (the example applies equally well to PySide)
from threading import Timer,Thread,Event


class OBCI_board_simulator():
    def __init__(self, filepath, FS=250.0,factorOriginalSpeed=1,skipSeconds=0,daisy=False):
        # QtCore.QThread.__init__(self)
        self.simulationCSVFilepath = filepath
        self.simulationFactorOriginalSpeed = factorOriginalSpeed
        self.simulateFromCSVskipSeconds = skipSeconds
        self.fs = FS
        self.daisy = daisy
        self.streaming = False
        self.read_csv_state_ready = False
        self.start_time = -100000
        self.lastSampleTime = -1
        self.sampleSupposedDelay = (1.0 / self.fs) / self.simulationFactorOriginalSpeed
        self.state_read_trough = False
        self.sample = OpenBCISample(0,(),())
        self.thread = None

    def getSkipedSeconds(self):
        return self.simulateFromCSVskipSeconds

    def getSkipedLines(self):
        return self.nSkipDataLines

    def read_csv_data_init(self):
        self.nSkipDataLines = int(self.simulateFromCSVskipSeconds*self.fs)

        self.read_csv_state_ready = False
        self.simulationCSVFile = open(self.simulationCSVFilepath, 'rb',buffering=20000000)  # opens the csv file
        # try:
            #self.inputiSample = 0
        self.reader = csv.reader(self.simulationCSVFile, delimiter=';')  # creates the reader object
        for row in self.reader:  # iterates the rows of the file in orders
            # print(row)  # prints each row
            if self.reader.line_num == 1:
                # index;time_sec;ringpar;ch1;ch2;ch3;ch4;ch5;ch6;ch7;ch8
                continue  # skip the first header line
            if self.reader.line_num <= self.nSkipDataLines + 1:
                continue
            else:
                break
        self.read_csv_state_ready = True
        # except:
            # f.close()  # closing
        return self.read_csv_state_ready

    def readSamples(self):
        if not self.state_read_trough:
            self.next_call_time = timeit.default_timer()
            try:
                while True:
                    if self.reading:
                        row = next(self.reader)
                        self.sample = namedtuple("Sample", "id channel_data time")
                        self.sample.time = timeit.default_timer()

                        self.sample.id = int(row[2])
                        if self.daisy and len(row) < 19:
                            print "you chose a 16 channel setup, but the file " + self.simulationCSVFilepath + " probably only a 8 channel recording or not the right file. Choose another 8 channel setup, or another 16 channel recording."
                            sys.exit(0)
                        if self.daisy:
                            self.sample.channel_data = [float(row[3]), float(row[4]), float(row[5]), float(row[6]),
                                                        float(row[7]), float(row[8]), float(row[9]), float(row[10]),
                                                        float(row[11]), float(row[12]), float(row[13]), float(row[14]),
                                                        float(row[15]), float(row[16]), float(row[17]), float(row[18])]
                            if len(row) == 22:
                                self.sample.aux_data = [float(row[19]), float(row[20]), float(row[21])]
                            else:
                                self.sample.aux_data = [0.0, 0.0, 0.0]
                        else:
                            self.sample.channel_data = [float(row[3]), float(row[4]), float(row[5]), float(row[6]),
                                                        float(row[7]), float(row[8]), float(row[9]), float(row[10])]
                            if len(self.sample.channel_data) == 14:
                                self.sample.aux_data = [float(row[11]), float(row[12]), float(row[13])]
                            else:
                                self.sample.aux_data = [0.0, 0.0, 0.0]

                    self.lastSampleTime = self.sample.time
                    for call in self.callback:
                        call(self.sample)


                    self.next_call_time = self.next_call_time + self.sampleSupposedDelay

                    #print "sleep delay: " + str(intendedSleep) + " next_call_time : " + str(self.next_call_time) + " sampleSupposedDelay: " + str(self.sampleSupposedDelay)

                    while self.next_call_time <= timeit.default_timer():
                        self.next_call_time = timeit.default_timer() + self.sampleSupposedDelay
                        #intendedSleep = self.next_call_time - timeit.default_timer()
                        self.reading = False

                    self.reading = True

                        # print "sleep delay: " + str(self.next_call_time - timeit.default_timer())
                    intendedSleep = self.next_call_time - timeit.default_timer()
                    time.sleep(max(intendedSleep, 0))







            except StopIteration:
                self.state_read_trough = True


    def start_streaming(self, callback, lapse=-1):

        if not self.streaming:
            self.streaming = True

        self.start_time = timeit.default_timer()

        # Enclose callback funtion in a list if it comes alone
        if not isinstance(callback, list):
            callback = [callback]
        self.callback = callback

        if not self.read_csv_state_ready:
            self.read_csv_data_init()

        self.state_read_trough = False

        self.reading = True

        self.thread = Thread(target=self.readSamples)
        self.thread.daemon = True
        self.thread.start()

        # self.thread = Timer(self.sampleSupposedDelay, self.readNextSample)
        # self.thread.start()

    def run(self):
        # Initialize
        self.read_csv_data_init()

    # def init(self):
    #     # Initialize
    #     self.read_csv_data_init()

    def cancel(self):
        self.simulationCSVFile.close()  # closing
        self.thread.cancel()