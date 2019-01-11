#import csv
#import timeit
import datetime
#from cls_algo1 import *
import pyedflib
import numpy as np
import atexit

class OpenBCIcsvCollect(object):
    def __exit__(self, exc_type, exc_val, ex_tb):
        self.deactivate()

    def __enter__(self):
        return self

    def __del__(self):
        self.deactivate()

    def __init__(self, FS, FS_ds, nChannels, file_name="collect", subfolder="data/rec/", delim=";", verbose=False, simulateFromCSV=False, doRealTimeStreaming=False, writeEDF=False, giveNameChannelsByMapping=False,subject="anonymous"):
        now = datetime.datetime.now()
        self.time_stamp = '%d-%d-%d_%d-%d-%d' % (now.year, now.month, now.day, now.hour, now.minute, now.second)
        self.subfolder = subfolder
        self.path_and_file_name = self.subfolder + file_name
        self.path_and_file_name_event = self.subfolder + file_name + ".csv.events"
        self.start_time = None #timeit.default_timer()
        self.delim = delim
        self.verbose = verbose
        self.writeIndex = None
        self.writeIndexEvent = None
        self.edfWriter = None
        self.f = None
        self.fevent = None
        self.isActivated = False
        self.doRealTimeStreaming=doRealTimeStreaming
        self.fs = FS
        self.FS_ds = FS_ds
        self.nChannels = nChannels
        self.writeEDF = writeEDF
        self.subject = subject
        self.edfSampleBuffer = []
        self.tempLastSampleID = 0
        self.tempLastSample = 0
        self.nNumberCorrectedSamples = 0
        atexit.register(self.deactivate)
        self.lastAcc = [0.0,0.0,0.0]
        self.simulateFromCSV = simulateFromCSV
        self.giveNameChannelsByMapping = giveNameChannelsByMapping


    def getWriteIndex(self):
        return self.writeIndex

    def getChannelByNumber(self,chNumber):
        if chNumber == 1:
            return "EMG"
        elif chNumber == 2:
            return "EOG"
        elif chNumber == 3:
            return "A1_inverted"
        elif chNumber == 4:
            return "A2_inverted"
        elif chNumber == 5:
            return "C3_inverted"
        elif chNumber == 6:
            return "C4_inverted"
        elif chNumber == 7:
            return "Trigger"
        elif chNumber == 8:
            return "ECG"
        elif chNumber == 9:
            return "F3_inverted"
        elif chNumber == 10:
            return "Fz_inverted"
        elif chNumber == 11:
            return "F4_inverted"
        elif chNumber == 12:
            return "P3_inverted"
        elif chNumber == 13:
            return "Pz_inverted"
        elif chNumber == 14:
            return "P4_inverted"
        elif chNumber == 15:
            return "O1_inverted"
        elif chNumber == 16:
            return "O2_inverted"

    def activate(self):
        self.writeIndex = 0
        self.writeIndexEvent = 0

        self.path_and_file_name = self.path_and_file_name + '.csv'
        print "Will export Data CSV to:", self.path_and_file_name
        # Open in append mode
        self.f = open(self.path_and_file_name, 'a', buffering=500000)

        if self.doRealTimeStreaming:
            self.path_and_file_name_event = self.path_and_file_name_event + '.csv'
            print "Will export Events CSV to:", self.path_and_file_name_event
            # Open in append mode
            self.fevent = open(self.path_and_file_name_event, 'a', buffering=100000)
            # if self.isActivated:
            #     self.fevent.write('#Streaming restarted at ' + self.time_stamp + '\n')
            # self.fevent.write('#Streaming started at ' + self.time_stamp + '\n')
            if self.writeEDF:
                self.edfWriter = pyedflib.EdfWriter(self.path_and_file_name + "_missing_samples_corrected" + ".bdf", self.nChannels+3, file_type=pyedflib.FILETYPE_BDFPLUS)
                self.edfWriter.set_number_of_annotation_signals(64)
                channel_info = {'label': 'ch', 'dimension': 'uV', 'sample_rate': int(round(self.fs)),
                                'physical_max': 187500, 'physical_min': -187500,
                                'digital_max': 8388607, 'digital_min': -8388608,
                                'prefilter': 'none', 'transducer': 'none'}
                channel_info_accel = {'label': 'acc', 'dimension': 'G', 'sample_rate': int(round(self.fs)),
                                'physical_max': 4, 'physical_min': -4,
                                'digital_max': 8388607, 'digital_min': -8388608,
                                'prefilter': 'none', 'transducer': 'none'}

                self.edfWriter.setTechnician('')
                self.edfWriter.setRecordingAdditional('')
                self.edfWriter.setPatientName(self.subject)
                self.edfWriter.setPatientCode('')
                self.edfWriter.setPatientAdditional('')
                self.edfWriter.setAdmincode('')
                self.edfWriter.setEquipment('OpenBCI')
                self.edfWriter.setGender(0)
                self.edfWriter.setBirthdate(datetime.date(2000, 1, 1))
                self.edfWriter.setStartdatetime(datetime.datetime.now())
                for iCh in range(0, self.nChannels):
                    self.edfWriter.setSignalHeader(iCh, channel_info.copy())
                    if self.giveNameChannelsByMapping:
                        self.edfWriter.setLabel(iCh, self.getChannelByNumber(iCh+1))

                    else:
                        self.edfWriter.setLabel(iCh, 'ch' + str(iCh+1))
                for iCh_acc in range(0, 3):
                    self.edfWriter.setSignalHeader(self.nChannels+iCh_acc, channel_info_accel.copy())
                    self.edfWriter.setLabel(self.nChannels+iCh_acc, 'acc' + str(iCh_acc+1))
                self.edfWriter.writeAnnotation(0, -1, u"signal_start")

        self.isActivated = True

    def isActivated(self):
        return self.isActivated

    def flush(self):
        if self.isActivated:
            self.f.flush()
            if self.doRealTimeStreaming:
                self.fevent.flush()


    def deactivate(self):
        if self.isActivated:
            self.f.flush()
            self.f.close()
            if self.edfWriter:
                self.edfWriter.close()
            if self.doRealTimeStreaming:
                self.fevent.flush()
                self.fevent.close()
            print "Closing, CSV saved to:", self.path_and_file_name

    def getCSVfileName(self):
        return self.path_and_file_name

    def fireStreamingStarted(self):
        if self.isActivated:
            if self.start_time is None:
                now = datetime.datetime.now()
                self.start_time = now
                temp_time_stamp = '%d-%d-%d_%d-%d-%d-%f' % (
                now.year, now.month, now.day, now.hour, now.minute, now.second, now.microsecond)
                self.f.write('#Streaming started at ' + temp_time_stamp + '\n')
                if self.doRealTimeStreaming:
                    self.fevent.write('#Streaming started at ' + temp_time_stamp + '\n')
                    self.fevent.write('index_event' + self.delim + 'datetime' + self.delim + 'index_write_sample' + self.delim + 'index_write_sample_input'  + self.delim + 'index_write_sample_input_second' + self.delim + 'time_since_start' + self.delim + 'sample_id' + self.delim + 'event' + '\n')

    def writeStimulusEvent(self,stimulusEvent):
        if self.doRealTimeStreaming:
            if not self.isActivated:
                self.activate()
            self.writeIndexEvent += 1
            # # print timeSinceStart|Sample Id
            if self.verbose:
                print("CSV event: %d | %s" % (stimulusEvent.getTime(), stimulusEvent.getString()))
            row = ''
            row += str(self.writeIndexEvent)
            row += self.delim
            row += str(stimulusEvent.getDatetimeStamp())
            row += self.delim
            row += str(self.writeIndex)
            row += self.delim
            row += str(stimulusEvent.getSampleWriteIndex())
            row += self.delim
            row += str(stimulusEvent.getSampleWriteIndex()/self.fs)
            row += self.delim
            row += str(stimulusEvent.getTime())
            row += self.delim
            row += str(stimulusEvent.getSampleID())
            row += self.delim
            row += str(stimulusEvent.getString())
            row += '\n'
            self.fevent.write(row)
            self.fevent.flush()

            if self.writeEDF:
                if self.simulateFromCSV:
                    self.edfWriter.writeAnnotation((self.writeIndex+self.nNumberCorrectedSamples+stimulusEvent.sampleWriteIndexOffset)/self.fs, -1, stimulusEvent.getString().encode("utf-8"))
                else:
                    self.edfWriter.writeAnnotation((stimulusEvent.getSampleWriteIndex()+self.nNumberCorrectedSamples)/self.fs, -1, stimulusEvent.getString().encode("utf-8"))
                #self.edfWriter.writeAnnotation((stimulusEvent.getSampleWriteIndex()+self.nNumberCorrectedSamples)/self.fs, -1, u"bla")
                #self.edfWriter.writeAnnotation(1, -1, u"bla1")



    # def writeStimulusEvents(self, stimEvents):
    #     if not self.isActivated:
    #         self.activate()
    #     # t = timeit.default_timer() - self.start_time
    #
    #     for s in stimEvents:
    #         self.writeIndexEvent += 1
    #         sname = s[0]
    #         stime = s[1]
    #         # print timeSinceStart|Sample Id
    #         if self.verbose:
    #             print("CSV event: %d | %s" % (stime, sname))
    #         row = ''
    #         row += str(self.writeIndexEvent)
    #         row += self.delim
    #         row += str(stime)
    #         row += self.delim
    #         row += str(sname)
    #         row += '\n'
    #         self.fevent.write(row)

    def diffSampleApprox(self,prevID,currentID):
        divider = 1
        if self.FS_ds or (self.nChannels == 16):
            divider = 2
        if currentID >= prevID:
            return (currentID-prevID)/divider
        else:
            return (256-prevID + currentID)/divider

    def writeSample(self, sample):
        if not self.isActivated:
            self.activate()
        # t = timeit.default_timer() - self.start_time

        if self.writeIndex < 1:
            self.tempLastSampleID = sample.id
            self.tempLastSample = sample
        self.writeIndex += 1

        # print timeSinceStart|Sample Id
        if self.verbose:
            print("CSV: %f | %d" % (sample.time, sample.id))
        row = ''
        row += str(self.writeIndex)
        row += self.delim
        row += str(sample.time)
        row += self.delim
        row += str(sample.id)
        row += self.delim
        for i in sample.channel_data:
            row += str(i)
            row += self.delim
        for i in sample.aux_data:
            row += str(i)
            row += self.delim
        # remove last comma
        row += '\n'
        self.f.write(row)

        if self.writeEDF:
            self.checkAndWriteEDF(sample)

    def checkAndWriteEDF(self,sample):
        tempdiff = self.diffSampleApprox(self.tempLastSampleID, sample.id)
        if tempdiff > 1:
            self.edfWriter.writeAnnotation((self.writeIndex + self.nNumberCorrectedSamples) / self.fs, -1,
                                           "interpolated_samples:" + str(tempdiff - 1))
            for iX in range(1, tempdiff):
                self.edfSampleBuffer.append(self.tempLastSample)
                self.nNumberCorrectedSamples += 1

        self.tempLastSampleID = sample.id
        self.tempLastSample = sample

        self.edfSampleBuffer.append(sample)

        if len(self.edfSampleBuffer) >= int(self.fs):
            ch = np.full((self.nChannels + 3, int(self.fs)), 0.0)
            for iSample, sample in enumerate(self.edfSampleBuffer):
                if iSample >= int(self.fs):
                    break
                for iChannel, val in enumerate(sample.channel_data):
                    ch[iChannel, iSample] = val

                acc = []
                for iChannel_acc, val in enumerate(sample.aux_data):
                    acc.append(val)

                if sum(acc) != 0.0:
                    for iChannel_acc, val in enumerate(sample.aux_data):
                        ch[self.nChannels + iChannel_acc, iSample] = acc[iChannel_acc]
                    self.lastAcc = acc
                else:
                    for iChannel_acc, val in enumerate(sample.aux_data):
                        ch[self.nChannels + iChannel_acc, iSample] = self.lastAcc[iChannel_acc]

            for iChannel_all in range(0, self.nChannels + 3):
                self.edfWriter.writePhysicalSamples(ch[iChannel_all,])

            self.edfSampleBuffer = self.edfSampleBuffer[int(self.fs):]

