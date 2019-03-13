#import csv
#import timeit
import datetime
#from cls_algo1 import *
import pyedflib
import numpy as np
import atexit
import math
import RealTimeFilter as rtf
import copy
from open_bci_v3 import OpenBCISample



class OpenBCIcsvCollect(object):
    def __exit__(self, exc_type, exc_val, ex_tb):
        self.deactivate()

    def __enter__(self):
        return self

    def __del__(self):
        self.deactivate()

    def __init__(self, FS, FS_ds, nChannels, file_name="collect", subfolder="data/rec/", delim=";", verbose=False, simulateFromCSV=False, doRealTimeStreaming=False, writeEDF=False, giveNameChannelsByMapping=False,subject="anonymous",prefilterEDF_hp=None, correctInvertedChannels=False):
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
        self.prefilterEDF_hp = prefilterEDF_hp
        self.EDF_Physical_max_microVolt = 3277
        self.EDF_Physical_min_microVolt = -self.EDF_Physical_max_microVolt

        self.realTimeFilterOrder = 1
        self.channelRealTimeFilters = []
        if self.prefilterEDF_hp is not None:
            for iCh in range(0, self.nChannels):
                self.channelRealTimeFilters.append(rtf.RealTimeFilterHighPassButter(self.prefilterEDF_hp, self.fs, self.realTimeFilterOrder))
        self.correctInvertedChannels = correctInvertedChannels

    def getWriteIndex(self):
        return self.writeIndex

    def isChannelInverted(self,chNumber):
        if chNumber in [3, 4, 5, 6, 9, 10, 11, 12, 13, 14, 15, 16]:
            return True
        return False

    def getChannelInversionMultiplicatorByChannelNumber(self, chNumber):
        if self.isChannelInverted(chNumber):
            return -1
        return 1

    def getChannelByNumber(self,chNumber):
        chName = ""
        if chNumber == 1:
            chName += "EMG"
        elif chNumber == 2:
            chName += "EOG"
        elif chNumber == 3:
            chName += "A1"
        elif chNumber == 4:
            chName += "A2"
        elif chNumber == 5:
            chName += "C3"
        elif chNumber == 6:
            chName += "C4"
        elif chNumber == 7:
            chName += "Trigger"
        elif chNumber == 8:
            chName += "ECG"
        elif chNumber == 9:
            chName += "F3"
        elif chNumber == 10:
            chName += "Fz"
        elif chNumber == 11:
            chName += "F4"
        elif chNumber == 12:
            chName += "P3"
        elif chNumber == 13:
            chName += "Pz"
        elif chNumber == 14:
            chName += "P4"
        elif chNumber == 15:
            chName += "O1"
        elif chNumber == 16:
            chName += "O2"
        if self.isChannelInverted(chNumber) and not self.correctInvertedChannels:
            chName += "_inverted"
        return chName

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

                EDF_format_extention = ".edf"
                EDF_format_filetype = pyedflib.FILETYPE_EDFPLUS
                temp_filterStringFileIndicator = "_prefiltered"
                temp_filterStringHeader = 'HP ' + str(self.prefilterEDF_hp) + ' Hz'
                if self.prefilterEDF_hp is None:
                    EDF_format_extention = ".bdf"
                    EDF_format_filetype = pyedflib.FILETYPE_BDFPLUS
                    temp_filterStringFileIndicator = "_unfiltered"
                    temp_filterStringHeader = 'none'

                self.edfWriter = pyedflib.EdfWriter(self.path_and_file_name + "_missing_samples_corrected" + temp_filterStringFileIndicator + EDF_format_extention, self.nChannels+3, file_type=EDF_format_filetype)
                self.edfWriter.set_number_of_annotation_signals(64)
                channel_info = {'label': 'ch', 'dimension': 'uV', 'sample_rate': int(round(self.fs)),
                                'physical_max': self.EDF_Physical_max_microVolt, 'physical_min': self.EDF_Physical_min_microVolt,
                                'digital_max': 32767, 'digital_min': -32767,
                                'prefilter': temp_filterStringHeader, 'transducer': 'none'}
                channel_info_accel = {'label': 'acc', 'dimension': 'G', 'sample_rate': int(round(self.fs)),
                                      'physical_max': 4, 'physical_min': -4,
                                      'digital_max': 32767, 'digital_min': -32767,
                                      'prefilter': 'none', 'transducer': 'none'}

                if EDF_format_extention == ".bdf":
                    channel_info = {'label': 'ch', 'dimension': 'uV', 'sample_rate': int(round(self.fs)),
                                    'physical_max': 187500, 'physical_min': -187500,
                                    'digital_max': 8388607, 'digital_min': -8388607,
                                    'prefilter': temp_filterStringHeader, 'transducer': 'none'}
                    channel_info_accel = {'label': 'acc', 'dimension': 'G', 'sample_rate': int(round(self.fs)),
                                    'physical_max': 4, 'physical_min': -4,
                                    'digital_max': 8388607, 'digital_min': -8388607,
                                    'prefilter': 'none', 'transducer': 'none'}

                self.edfWriter.setTechnician('')
                self.edfWriter.setRecordingAdditional('COsleep')
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

    def diffSampleApprox(self,prevID,currentID,prevTime,currentTime,fs):
        divider = 1
        if self.FS_ds or (self.nChannels == 16):
            divider = 2

        diff_samples = 0
        diff_samples_id = 0
        approx_samples_diff = (currentTime - prevTime) * (self.fs * divider)
        #diff_samples += int(math.floor(approx_samples_diff / 256.0) * 256.0 / divider)
        if math.floor(approx_samples_diff) > 251.0:
            diff_samples += int(math.floor(approx_samples_diff) / divider)
        else:
            if currentID >= prevID:
                diff_samples_id += (currentID-prevID)/divider
            else:
                diff_samples_id += (256-prevID + currentID)/divider
        return diff_samples+diff_samples_id



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

    def prefilterEDF(self, sample):
        for iCh in range(0, sample.channel_data.__len__()):
            sample.channel_data[iCh] = self.channelRealTimeFilters[iCh].fitlerNextSample(sample.channel_data[iCh])
            if sample.channel_data[iCh] > self.EDF_Physical_max_microVolt:
                sample.channel_data[iCh] = float(self.EDF_Physical_max_microVolt)
            if sample.channel_data[iCh] < self.EDF_Physical_min_microVolt:
                sample.channel_data[iCh] = float(self.EDF_Physical_min_microVolt)
        return sample

    def correctInvertedChannelsInSample(self, sample):
        for iCh in range(0, sample.channel_data.__len__()):
            sample.channel_data[iCh] = sample.channel_data[iCh]*self.getChannelInversionMultiplicatorByChannelNumber(iCh + 1)
        return sample

    def checkAndWriteEDF(self, sample):
        # sample = copy.deepcopy(sample)
        sample = OpenBCISample(copy.deepcopy(sample.id),
                               copy.deepcopy(sample.channel_data),
                               copy.deepcopy(sample.aux_data),
                               copy.deepcopy(sample.time))
        #sample.channel_data[0] = 14;
        tempdiff = self.diffSampleApprox(self.tempLastSampleID, sample.id, self.tempLastSample.time, sample.time, self.fs)
        if tempdiff > 1:
            self.edfWriter.writeAnnotation((self.writeIndex + self.nNumberCorrectedSamples) / self.fs, -1,
                                           "interpolated_samples:" + str(tempdiff - 1))
            for iX in range(1, tempdiff):
                self.edfSampleBuffer.append(self.tempLastSample)
                self.nNumberCorrectedSamples += 1

        self.tempLastSampleID = sample.id
        self.tempLastSample = sample

        if self.correctInvertedChannels:
            sample = self.correctInvertedChannelsInSample(sample)

        if self.prefilterEDF_hp is not None:
            sample = self.prefilterEDF(sample)

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

