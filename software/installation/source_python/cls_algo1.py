import collections
import math
from scipy.signal import hilbert
import csv
import pygame
import pygame.locals
import timeit
import datetime
import RealTimeFilter as rtf
rasperryPi = False
if rasperryPi:
    import RPi.GPIO as GPIO
import numpy as np
import time
from PyQt4 import QtGui, QtCore
import Dialogs
import threading
import os.path
import Montage


class CLSalgo1(QtCore.QThread):

    def __init__(self, fs=250.0,
                 useDaisy=False,
                 FS_ds=False,
                 updateSendOutDelaySeconds=0.2,
                 montage=None,
                 realTimeFilterOrder=1,
                 realTimeFilterOrderSpindles=100,
                 isStimulationTurnedOn=False,
                 stimulusPlayer=None,
                 soundBufferSize=4096,
                 isSham=False,
                 isClosedLoop=False,
                 isClosedLoopRepeatSequence=True,
                 ThresholdDownStateDetectionPassBelow=-80.0,
                 waitForFurtherDipInThreshold=True,
                 ThresholdUpStateDetectionPassAbove=-20.0,
                 ThresholdUpStateDetectionPassBelow=200.0,
                 firstDownToUpstateDelayInSec=0.508,
                 # secondUpToUpstateDelayInSec = 1.075,
                 threshold_EMGSignal_from_away_zero_disengage_algo=25,  # in microVolt
                 threshold_EEGSignal_from_away_zero_disengage_algo=300,
                 threshold_EOGSignal_from_away_zero_disengage_algo=300,
                 doSpindleHighlight=True,
                 filterHP_EEG_spindle_freq=11,
                 filterLP_EEG_spindle_freq=15,
                 spindle_amplitude_threshold_detect_microVolts=15,
                 spindle_amplitude_threshold_begin_end_microVolts=10,
                 spindle_amplitude_max_microVolts=120,
                 spindle_min_duration_seconds=0.5,
                 spindle_max_duration_seconds=2):

        QtCore.QThread.__init__(self)

        self.isStimulationTurnedOn = isStimulationTurnedOn


        if rasperryPi:
            GPIO.setmode(GPIO.BCM)
            GPIO.setup(23, GPIO.OUT)

            # test the GPIO with two blinks
            GPIO.output(23, GPIO.HIGH)
            time.sleep(0.5)
            GPIO.output(23, GPIO.LOW)
            time.sleep(0.5)
            GPIO.output(23, GPIO.HIGH)
            time.sleep(0.5)
            GPIO.output(23, GPIO.LOW)

        self.fs = fs

        self.montage = montage

        self.EEG_SIGNAL_CORRECTION_FACTOR = -1;
        self.updateSendOutDelaySeconds = updateSendOutDelaySeconds
        self.updateViewSampleUpdate = int(math.ceil(self.updateSendOutDelaySeconds * self.fs))
        self.updateViewSampleCounter = 1

        self.useDaisy = useDaisy
        self.FS_ds = FS_ds

        self.channelEEGrefs = self.montage.getRerefChannelNumbers()

        self.channelEEG = self.montage.getGUIchannelNumber("EEG")
        self.channelEMG = self.montage.getGUIchannelNumber("EMG")
        self.channelEOG = self.montage.getGUIchannelNumber("EOG")

        self.data_ring_buffer_length_seconds = 600.0
        self.data_ring_buffer_length_samples = int(round(self.data_ring_buffer_length_seconds * self.fs))
        self.dataEEG = collections.deque(maxlen=self.data_ring_buffer_length_seconds)
        self.dataEOG = collections.deque(maxlen=self.data_ring_buffer_length_seconds)
        self.dataEMG = collections.deque(maxlen=self.data_ring_buffer_length_seconds)

        self.signal_ring_buffer_length_seconds = 30.0
        self.signal_ring_buffer_length_samples = int(round(self.signal_ring_buffer_length_seconds * self.fs))
        self.signalEEG = collections.deque(maxlen=self.signal_ring_buffer_length_samples)
        self.signalEMG = collections.deque(maxlen=self.signal_ring_buffer_length_samples)
        self.signalEOG = collections.deque(maxlen=self.signal_ring_buffer_length_samples)
        self.signalEEG_spindles = collections.deque(maxlen=self.signal_ring_buffer_length_samples)

        self.signalTrackingEEG = collections.deque(maxlen=self.signal_ring_buffer_length_samples)
        self.signalTrackingEMG = collections.deque(maxlen=self.signal_ring_buffer_length_samples)
        self.signalTrackingEOG = collections.deque(maxlen=self.signal_ring_buffer_length_samples)

        self.status_doTracking = False
        self.status_doStimulation = False
        self.status_engagedStimulation = False

        self.realTimeFilterOrder = realTimeFilterOrder
        self.realTimeFilterOrderSpindles = realTimeFilterOrderSpindles

        self.isSham = isSham

        self.isClosedLoop = isClosedLoop
        self.isClosedLoopRepeatSequence = isClosedLoopRepeatSequence

        self.doSpindleHighlight = doSpindleHighlight
        self.filterHP_EEG_spindle_freq = filterHP_EEG_spindle_freq
        self.filterLP_EEG_spindle_freq = filterLP_EEG_spindle_freq

        self.spindle_amplitude_threshold_detect_microVolts = spindle_amplitude_threshold_detect_microVolts
        self.spindle_amplitude_threshold_begin_end_microVolts = spindle_amplitude_threshold_begin_end_microVolts
        self.spindle_amplitude_max_microVolts = spindle_amplitude_max_microVolts

        self.spindle_min_duration_samples = int(round(self.fs * spindle_min_duration_seconds))
        self.spindle_max_duration_samples = int(round(self.fs * spindle_max_duration_seconds))

        self.startTime = timeit.default_timer()
        self.signalTime = np.linspace(0, self.startTime + self.signal_ring_buffer_length_seconds, self.signal_ring_buffer_length_samples)
        self.lastSampleTime = 0.0

        self.lastStimSample = 1

        self.ThresholdDownStateDetectionPassBelow = ThresholdDownStateDetectionPassBelow
        self.waitForFurtherDipInThreshold = waitForFurtherDipInThreshold
        self.ThresholdUpStateDetectionPassAbove = ThresholdUpStateDetectionPassAbove
        self.ThresholdUpStateDetectionPassBelow = ThresholdUpStateDetectionPassBelow
        self.threshold = ThresholdDownStateDetectionPassBelow

        self.stimulusPlayer = stimulusPlayer

        self.status_forcedStimEngagement = False
        if isStimulationTurnedOn:
            if self.stimulusPlayer is not None:
                self.maxFreqDelayInHz = 2.0
                self.minFreqDelayInSec = 1.0 / self.maxFreqDelayInHz

                self.PauseStimulationForPeriodAfterLastStimInSec = 0.0
                self.PostMakerDelayInSec = firstDownToUpstateDelayInSec - self.stimulusPlayer.soundlatency_seconds
                self.PauseStimulationForPeriodAfterLastStimInSecInSample = int(round(self.fs * self.PauseStimulationForPeriodAfterLastStimInSec))

                self.maxFreqDelayInSample = self.fs / self.maxFreqDelayInHz

                self.postMarkerDelaySample = int(round(self.fs * self.PostMakerDelayInSec))
                self.updateThresholdIntervalSec = 2.0
                self.updateThresholdIntervalSample = int(round(self.fs * self.updateThresholdIntervalSec))
                self.updateThresholdTimeWindowSec = 5.0
                self.updateThresholdTimeWindowSample = int(round(self.fs * self.updateThresholdTimeWindowSec))

                self.tempMinVal = self.threshold
                self.tempMinValSample = 1
                self.hasThresholdBeenCrossed = False
                self.marker = list()
                self.markerWriteIndex = list()
                self.markerSampleId = list()
                self.marker.append(0)  # dummy append
                self.markerWriteIndex.append(0)  # dummy append
                self.markerSampleId.append(0)  # dummy append
                self.postMarker = list()
                self.postPostMarker = list()
                self.iMarker = 0
                self.iPostMarker = 0
                self.iPostPostMarker = 0
                self.readyPostPostMarker = False
                self.lastThresholdCrossSample = 0
                self.sampleDisengagedEMG = 0
                self.waitReleaseTightEMGmonitoringSeconds = 150
                self.waitReleaseTightEMGmonitoringSamples = int(round(self.fs * self.waitReleaseTightEMGmonitoringSeconds))
                self.sampleDisengagedEMG = -self.waitReleaseTightEMGmonitoringSamples
                self.is_threshold_EMGSignal_from_away_zero_disengage_algo_lowered = False

        self.iSample = 0
        self.TrackingStartSample = -1
        self.StimulationStartSample = -1
        self.currentSampleID = -1
        self.currentSampleWriteIndex = -1

        self.OBCI_sampleid = -1

        self.trackingBufferBeforeStimulationSeconds = 10.0
        self.trackingBufferBeforeStimulationSample = int(round(self.fs * self.trackingBufferBeforeStimulationSeconds))

        self.startBufferBeforeStimulationSeconds = 5.0
        self.startBufferBeforeStimulationSample = int(round(self.fs * self.startBufferBeforeStimulationSeconds))

        self.signal_stim_preview_before_event_seconds = 1
        self.signal_stim_preview_after_event_seconds = 3

        self.signal_stim_preview_event_seconds = self.signal_stim_preview_before_event_seconds + self.signal_stim_preview_after_event_seconds

        self.signal_stim_preview_before_event_samples = int(round(self.signal_stim_preview_before_event_seconds * self.fs))
        self.signal_stim_preview_after_event_samples = int(round(self.signal_stim_preview_after_event_seconds * self.fs))

        self.signal_stim_preview_event_length_samples = int(round(self.signal_stim_preview_event_seconds * self.fs))

        self.ERPtime = np.linspace(-self.signal_stim_preview_before_event_seconds,
                                   self.signal_stim_preview_after_event_seconds, self.signal_stim_preview_event_length_samples)

        self.signalERP = np.repeat(0, self.signal_stim_preview_event_length_samples)
        self.signalERPpremature = np.repeat(0, self.signal_stim_preview_event_length_samples)
        self.signalERPavgCount = 0
        self.signalERPavgCountSequence = np.repeat(0.000000001, self.signal_stim_preview_event_length_samples)
        self.signalERPsum = np.repeat(0, self.signal_stim_preview_event_length_samples)
        self.signalERPavg = np.repeat(0, self.signal_stim_preview_event_length_samples)

        self.signalERPavgMaxCount = 10
        self.signalERPavgTail = collections.deque(maxlen=self.signalERPavgMaxCount + 1000)

        self.ERP_samples_to_update = 0
        self.ERPcaptureIsComplete = True
        self.ERPcapturePrematureFired = False

        self.filterHP_EEG_freq = 0.16
        self.filterHP_EOG_freq = 0.16
        self.filterHP_EMG_freq = 10

        self.filterLP_EEG_freq = 30.0
        self.filterLP_EOG_freq = 30.0

        self.filterHP_EEG = rtf.RealTimeFilterHighPassButter(self.filterHP_EEG_freq, self.fs, self.realTimeFilterOrder)
        self.filterLP_EEG = rtf.RealTimeFilterLowPassButter(self.filterLP_EEG_freq, self.fs, self.realTimeFilterOrder)
        self.filterHP_EOG = rtf.RealTimeFilterHighPassButter(self.filterHP_EOG_freq, self.fs, self.realTimeFilterOrder)
        self.filterLP_EOG = rtf.RealTimeFilterLowPassButter(self.filterLP_EOG_freq, self.fs, self.realTimeFilterOrder)
        self.filterHP_EMG = rtf.RealTimeFilterHighPassButter(self.filterHP_EMG_freq, self.fs, self.realTimeFilterOrder)

        self.filterBP_EEG_spindle = rtf.RealTimeFilterBandPassFIR(self.filterHP_EEG_spindle_freq, self.filterLP_EEG_spindle_freq, self.fs, self.realTimeFilterOrderSpindles)
        self.filterBP_EEG_spindle_delayInSamples = self.filterBP_EEG_spindle.getDelayInSamples()


        self.status_testing = False
        self.testingStimulusMinIntervalSeconds = 0.0
        self.testingStimulusMinIntervalSample = int(round(self.fs * self.testingStimulusMinIntervalSeconds))
        self.testing_trigger_ready = False

        self.threshold_EMGSignal_from_away_zero_disengage_algo = threshold_EMGSignal_from_away_zero_disengage_algo  # in microVolt

        self.threshold_EEGSignal_from_away_zero_disengage_algo = threshold_EEGSignal_from_away_zero_disengage_algo  # in microVolt

        self.threshold_EOGSignal_from_away_zero_disengage_algo = threshold_EOGSignal_from_away_zero_disengage_algo  # in microVolt

        self.read_csv_state_ready = False

        self.csv = None
        self.board = None
        self.sv = None
        self.simulateFromCSV = False
        self.simulationCSVFilepath = None
        self.simulationFactorOriginalSpeed = None
        self.simulateFromCSVskipSeconds = 0

        self.EventFired = StimulusEventList(self.csv)
        if self.simulateFromCSV:
            self.EventFired.setStartTimeOffsetSeconds(self.csv_simulator.getSkipedSeconds())

        if self.isStimulationTurnedOn:
            self.stimulusPlayer.setAlgo(self)
            self.stimulusPlayer.updateClosedLoopParameter()

    def updateClosedLoopParameter(self,firstDownToUpstateDelayInSec,
                                  ThresholdDownStateDetectionPassBelow,
                                  waitForFurtherDipInThreshold,
                                  ThresholdUpStateDetectionPassAbove,
                                  ThresholdUpStateDetectionPassBelow):
        if self.isStimulationTurnedOn and self.isClosedLoop and not(self.isClosedLoopRepeatSequence):

            self.firstDownToUpstateDelayInSec = max(round(self.stimulusPlayer.soundlatency_seconds, 3) + (1 / self.fs) + 0.001, firstDownToUpstateDelayInSec)

            self.PostMakerDelayInSec = self.firstDownToUpstateDelayInSec - self.stimulusPlayer.soundlatency_seconds
            self.postMarkerDelaySample = int(round(self.fs * self.PostMakerDelayInSec))

            self.ThresholdDownStateDetectionPassBelow = ThresholdDownStateDetectionPassBelow
            self.waitForFurtherDipInThreshold = waitForFurtherDipInThreshold
            self.ThresholdUpStateDetectionPassAbove = ThresholdUpStateDetectionPassAbove
            self.ThresholdUpStateDetectionPassBelow = ThresholdUpStateDetectionPassBelow

            self.hasThresholdBeenCrossed = False
            self.readyPostPostMarker = False

    def updatePauseStimulationForPeriodAfterLastStimInSec(self):
        if self.isStimulationTurnedOn:
            if self.stimulusPlayer is not None:
                self.PauseStimulationForPeriodAfterLastStimInSec = self.stimulusPlayer.getCurrentPlayedStimWaitPlayNextStimSeconds()
                self.PauseStimulationForPeriodAfterLastStimInSecInSample = int(round(self.fs * self.PauseStimulationForPeriodAfterLastStimInSec))

    def doTracking(self):
        self.status_doTracking = True
        self.TrackingStartSample = self.iSample
        return self.status_doTracking

    def isTracking(self):
        return self.status_doTracking

    def doStimulation(self):
        if self.status_doTracking:
            self.status_doStimulation = True
            self.StimulationStartSample = self.iSample
            print("ca started Stimulation")
            self.EventFired.appendEvent("Stim-Started", self.currentSampleID, self.currentSampleWriteIndex)
        return self.status_doStimulation

    def pauseStimulation(self):
        self.status_doStimulation = False
        print("ca paused Stimulation")
        self.EventFired.appendEvent("Stim-paused", self.currentSampleID, self.currentSampleWriteIndex)
        return self.status_doStimulation

    def isStimulating(self):
        return self.status_doStimulation

    def isReadyForStimulation(self):
        return self.status_doTracking and ((self.iSample - self.TrackingStartSample) > self.trackingBufferBeforeStimulationSample) and ((self.iSample - self.StimulationStartSample) > self.startBufferBeforeStimulationSample)

    def doTesting(self, isTestingStatus=True):
        self.status_testing = isTestingStatus
        return self.status_testing

    def isTesting(self):
        return self.status_testing

    def fireERPcapture(self):
        self.ERPcapturePrematureFired = self.ERP_samples_to_update > 0
        if self.ERPcapturePrematureFired:
            self.signalERPpremature = self.signalERP.__copy__()
        prev_signalEEG = np.array(self.signalEEG)[
                         (len(self.signalEEG) - self.signal_stim_preview_before_event_samples):len(self.signalEEG)]
        self.signalERP[0:len(self.signalEEG)] = 0
        self.signalERP[0:self.signal_stim_preview_before_event_samples] = prev_signalEEG
        self.ERP_samples_to_update = self.signal_stim_preview_event_length_samples - len(prev_signalEEG)
        self.ERPcaptureIsComplete = False


    def fireTestStimulus(self):
        fired_a_stimulus = False
        if not (self.isSham and self.status_doStimulation):
            self.stimulusPlayer.playTestStimulus()
            self.EventFired.appendEvent("TestStim", self.currentSampleID, self.currentSampleWriteIndex)
        fired_a_stimulus = True
        if fired_a_stimulus:
            self.fireERPcapture()
            # print("-- Test Stimulus --")
        return fired_a_stimulus

    def handleCheckEEG(self):
        self.EventFired.appendEvent("EEG-check", self.currentSampleID, self.currentSampleWriteIndex)

    def handleLightsOff(self):
        self.EventFired.appendEvent("Ligths-Off", self.currentSampleID, self.currentSampleWriteIndex)

    def handleLightsOn(self):
        self.EventFired.appendEvent("Lights-On", self.currentSampleID, self.currentSampleWriteIndex)

    def handleReconnect(self):
        self.EventFired.appendEvent("Reconnect-attemp", self.currentSampleID, self.currentSampleWriteIndex)
        self.board.reconnect()
        self.EventFired.appendEvent("Reconnect-attemp-finished", self.currentSampleID, self.currentSampleWriteIndex)



    def setAutoStimEngaged(self):
        self.EventFired.appendEvent("Auto-Stim-Engage-turned-on", self.currentSampleID, self.currentSampleWriteIndex)
        self.status_forcedStimEngagement = False

    def setForcedStimEngaged(self):
        self.EventFired.appendEvent("Forced-Stim-Engage-turned-on", self.currentSampleID, self.currentSampleWriteIndex)
        self.status_forcedStimEngagement = True

    def changeEEGchannel(self,new_channelEEG):
        self.channelEEG = new_channelEEG
        self.EventFired.appendEvent("Channel-EEG-changed-to-" + str(new_channelEEG) + "-" + self.montage.getChannelLabelByChannelNumber(new_channelEEG), self.currentSampleID, self.currentSampleWriteIndex)

    def changeSoundRiseFromBaseLeveldB(self, new_sound_rise_from_base_level_db):
        self.stimulusPlayer.changeSoundRiseFromBaseLeveldB(new_sound_rise_from_base_level_db)

        new_final_sound_base_level_db_str = self.stimulusPlayer.final_sound_base_level_db_str

        sound_rise_from_base_level_db_str = str(abs(self.stimulusPlayer.sound_rise_from_base_level_db))
        if self.stimulusPlayer.sound_rise_from_base_level_db < 0:
            sound_rise_from_base_level_db_str = "minus" + sound_rise_from_base_level_db_str
        else:
            sound_rise_from_base_level_db_str = "plus" + sound_rise_from_base_level_db_str

        self.EventFired.appendEvent("Sound-Base-Level-Change_abs." + new_final_sound_base_level_db_str + "_rise." + str(sound_rise_from_base_level_db_str), self.currentSampleID,
                                    self.currentSampleWriteIndex)

    def checkStimulationConditions(self):
        stimulation_engaged_pre = self.status_engagedStimulation
        if self.status_doTracking and self.status_doStimulation and (not self.status_forcedStimEngagement):
            if self.status_engagedStimulation and self.is_threshold_EMGSignal_from_away_zero_disengage_algo_lowered and ((self.iSample - self.sampleDisengagedEMG) > self.waitReleaseTightEMGmonitoringSamples):
                self.threshold_EMGSignal_from_away_zero_disengage_algo = self.threshold_EMGSignal_from_away_zero_disengage_algo * 3.0
                self.is_threshold_EMGSignal_from_away_zero_disengage_algo_lowered = False
            if any(abs(np.array(self.signalTrackingEMG)) > self.threshold_EMGSignal_from_away_zero_disengage_algo) or \
                    any(abs(np.array(self.signalTrackingEEG)) > self.threshold_EEGSignal_from_away_zero_disengage_algo) or \
                    any(abs(np.array(self.signalTrackingEOG)) > self.threshold_EOGSignal_from_away_zero_disengage_algo):
                if self.status_engagedStimulation and not(self.is_threshold_EMGSignal_from_away_zero_disengage_algo_lowered):
                    self.is_threshold_EMGSignal_from_away_zero_disengage_algo_lowered = True
                    self.threshold_EMGSignal_from_away_zero_disengage_algo = self.threshold_EMGSignal_from_away_zero_disengage_algo/3.0
                    self.sampleDisengagedEMG = self.iSample
                self.status_engagedStimulation = False
                # print "tracking_disengage"
            elif any(np.array(self.signalTrackingEEG) < self.threshold) and self.isClosedLoop:
                self.status_engagedStimulation = True
                # print "closedloop_engage"
            else:
                self.status_engagedStimulation = True
                # print "just_engage"
        elif self.status_doTracking and self.status_doStimulation and self.status_forcedStimEngagement:
            self.status_engagedStimulation = True
            # print "forced_engage"
        else:
            self.status_engagedStimulation = False
            # print "unexpected_disengage"

        if stimulation_engaged_pre is not self.status_engagedStimulation:
            if stimulation_engaged_pre:
                self.EventFired.appendEvent("Stim-Disengaged", self.currentSampleID, self.currentSampleWriteIndex)
                self.readyPostPostMarker = False
                self.hasThresholdBeenCrossed = False
            else:
                self.EventFired.appendEvent("Stim-Engaged", self.currentSampleID, self.currentSampleWriteIndex)
                if self.isClosedLoopRepeatSequence:
                    self.stimulusPlayer.resetPlayList()

    def __input_algo_OpenLoop(self, eegsample, emgsample, eogsample):
        if self.status_doStimulation and self.isReadyForStimulation():
            self.fireStimulus()

    def __input_algo_ClosedLoop(self, eegsample, emgsample, eogsample):
        if self.status_doStimulation and self.isReadyForStimulation():
            self.stimulusPlayer.tryIteratePlaylist()
            val = eegsample
            if not self.hasThresholdBeenCrossed:
                self.tempMinVal = self.threshold
            if val < self.threshold and not(self.hasThresholdBeenCrossed) and not(self.readyPostPostMarker) \
                    and ((self.iSample - self.lastThresholdCrossSample) > self.postMarkerDelaySample)\
                    and (self.iSample - self.lastStimSample) >= (self.PauseStimulationForPeriodAfterLastStimInSecInSample-self.postMarkerDelaySample):
                self.tempMinVal = val
                self.hasThresholdBeenCrossed = True
                self.lastThresholdCrossSample = self.iSample
                self.marker[self.iMarker] = self.iSample
                self.markerWriteIndex[self.iMarker] = self.currentSampleWriteIndex
                self.markerSampleId[self.iMarker] = self.currentSampleID
                # set marker
            if (val <= self.tempMinVal) and self.hasThresholdBeenCrossed and self.waitForFurtherDipInThreshold:
                self.tempMinVal = val
                self.marker[self.iMarker] = self.iSample
                self.markerWriteIndex[self.iMarker] = self.currentSampleWriteIndex
                self.markerSampleId[self.iMarker] = self.currentSampleID
                self.lastThresholdCrossSample = self.iSample
            if self.hasThresholdBeenCrossed and not (self.readyPostPostMarker) and (
                    (self.iSample - self.lastThresholdCrossSample) == self.postMarkerDelaySample) and \
                    ((self.iSample - self.lastStimSample) >= self.PauseStimulationForPeriodAfterLastStimInSecInSample):
                # fire the Trigger for first upstate
                if (val >= self.ThresholdUpStateDetectionPassAbove) and (val <= self.ThresholdUpStateDetectionPassBelow):
                    self.fireStimulus()
                    self.iMarker += 1
                    self.marker.append(0)  # dummy append
                    self.markerWriteIndex.append(0)  # dummy append
                    self.markerSampleId.append(0)  # dummy append
            if ((self.iSample - self.lastThresholdCrossSample) == self.postMarkerDelaySample):
                self.hasThresholdBeenCrossed = False
            if self.readyPostPostMarker and not (self.hasThresholdBeenCrossed):
                # fire the Trigger for second upstate
                self.fireStimulus()
            #             if ((self.iSample % self.updateThresholdIntervalSample) == 0) and ((self.iSample - self.marker[self.iMarker]) > self.postMarkerDelaySample):
            if ((self.iSample % self.updateThresholdIntervalSample) == 0):
                # print "threshold calibrated"
                tempIndexmin1 = len(self.signalTrackingEEG) - min(self.updateThresholdTimeWindowSample, len(self.signalTrackingEEG))
                tempMinSignal = self.ThresholdDownStateDetectionPassBelow
                for iTempSample in range(tempIndexmin1, len(self.signalTrackingEEG) - 1):
                    tempMinSignal = min(tempMinSignal, self.signalTrackingEEG[iTempSample])
                self.threshold = min(self.ThresholdDownStateDetectionPassBelow, tempMinSignal)

    def fireStimulus(self):
        fired_a_stimulus = False
        fired_a_nonERP_stimulus = False
        if self.isClosedLoop:
            if self.isClosedLoopRepeatSequence and self.readyPostPostMarker:
                fired_a_stimulus = self.stimulusPlayer.checkAndPlayNextStimuliInPlayListWithoutIterate()
                if fired_a_stimulus:
                    self.updatePauseStimulationForPeriodAfterLastStimInSec()
                    self.lastStimSample = self.iSample
                    se = self.EventFired.appendEvent(self.stimulusPlayer.getCurrentLoadedPlayStimulusID(), self.currentSampleID, self.currentSampleWriteIndex)
                    se = self.EventFired.appendEvent(">(" + self.stimulusPlayer.getCurrentLoadedPlayStimulusID() + ")<", self.currentSampleID,
                                                     self.currentSampleWriteIndex + int(round(self.fs*self.stimulusPlayer.soundlatency_seconds)),
                                                     timeSinceStartOffsetSeconds=self.stimulusPlayer.soundlatency_seconds,sampleWriteIndexOffset=int(round(self.fs*self.stimulusPlayer.soundlatency_seconds)))
                    fired_a_stimulus = True
                    fired_a_nonERP_stimulus = True
                    self.readyPostPostMarker = False
                    # print("-- Stimulus 2 --")
            elif self.hasThresholdBeenCrossed and not (self.readyPostPostMarker):
                fired_a_stimulus = self.stimulusPlayer.checkAndPlayNextStimuliInPlayListWithoutIterate()
                if fired_a_stimulus:
                    self.updatePauseStimulationForPeriodAfterLastStimInSec()
                    self.lastStimSample = self.iSample
                    if (self.stimulusPlayer.playListLength == 2) and self.isClosedLoopRepeatSequence:
                        self.readyPostPostMarker = True
                    else:
                        self.readyPostPostMarker = False
                    se = self.EventFired.appendEvent("Th", self.markerSampleId[self.iMarker], self.markerWriteIndex[self.iMarker],
                                                     timeSinceStartOffsetSeconds=(self.markerWriteIndex[self.iMarker] - self.currentSampleWriteIndex) / self.fs,sampleWriteIndexOffset=(self.markerWriteIndex[self.iMarker] - self.currentSampleWriteIndex))
                    se = self.EventFired.appendEvent(self.stimulusPlayer.getCurrentLoadedPlayStimulusID(), self.currentSampleID, self.currentSampleWriteIndex)
                    se = self.EventFired.appendEvent(">(" + self.stimulusPlayer.getCurrentLoadedPlayStimulusID() + ")<", self.currentSampleID,
                                                     self.currentSampleWriteIndex + int(round(self.fs*self.stimulusPlayer.soundlatency_seconds)),
                                                         timeSinceStartOffsetSeconds=self.stimulusPlayer.soundlatency_seconds,sampleWriteIndexOffset=int(round(self.fs*self.stimulusPlayer.soundlatency_seconds)))
                    # print("-- Stimulus 1 --")
        else:  # OpenLoop
            fired_a_stimulus = self.stimulusPlayer.checkAndPlayNextStimuliInPlayListWithIterate()
            if fired_a_stimulus:
                se = self.EventFired.appendEvent(self.stimulusPlayer.getCurrentLoadedPlayStimulusID(), self.currentSampleID, self.currentSampleWriteIndex)
                se = self.EventFired.appendEvent(">(" + self.stimulusPlayer.getCurrentLoadedPlayStimulusID() + ")<", self.currentSampleID,
                                                 self.currentSampleWriteIndex + int(round(self.fs*self.stimulusPlayer.soundlatency_seconds)),
                                                 timeSinceStartOffsetSeconds=self.stimulusPlayer.soundlatency_seconds,sampleWriteIndexOffset=int(round(self.fs*self.stimulusPlayer.soundlatency_seconds)))

        if fired_a_stimulus and not fired_a_nonERP_stimulus:
            self.fireERPcapture()

        return fired_a_stimulus

    def __input_testing(self, eegsample, emgsample, eogsample):
        if self.status_doTracking:

            self.iSample += 1

            val = eegsample
            if self.status_doStimulation:
                if len(self.marker) > 0:
                    tempPrevMarkerEnoughAway = ((self.iSample - self.marker[len(self.marker) - 1]) >= self.testingStimulusMinIntervalSample)
                else:
                    tempPrevMarkerEnoughAway = True
                if (val > self.threshold):
                    if self.testing_trigger_ready and tempPrevMarkerEnoughAway:
                        self.marker.append(0)  # dummy append
                        self.marker[self.iMarker] = self.iSample
                        self.fireTestStimulus()
                        self.testing_trigger_ready = False
                        self.iMarker += 1
                if val < self.threshold:
                    self.testing_trigger_ready = True
        return

    def input(self, eegsample, emgsample, eogsample):
        self.iSample += 1
        self.dataEEG.append(eegsample)
        self.dataEOG.append(eogsample)
        self.dataEMG.append(emgsample)

        eegsample_filt = self.filterHP_EEG.fitlerNextSample(eegsample)
        eegsample_filt = self.filterLP_EEG.fitlerNextSample(eegsample_filt)

        eogsample_filt = self.filterHP_EOG.fitlerNextSample(eogsample)
        eogsample_filt = self.filterLP_EOG.fitlerNextSample(eogsample_filt)

        emgsample_filt = self.filterHP_EMG.fitlerNextSample(emgsample)

        # self.data_filtered.append(eegsample)

        self.signalEEG.append(eegsample_filt)
        self.signalEMG.append(emgsample_filt)
        self.signalEOG.append(eogsample_filt)

        if self.doSpindleHighlight:
            eegsample_filt_spindle = self.filterBP_EEG_spindle.fitlerNextSample(eegsample_filt)
            self.signalEEG_spindles.append(eegsample_filt_spindle)

        self.ERP_samples_to_update -= 1
        if self.ERP_samples_to_update >= 0:
            self.signalERP[self.signal_stim_preview_event_length_samples - self.ERP_samples_to_update - 1] = eegsample_filt
        doUpdate = False
        if (self.ERP_samples_to_update <= 0) and (not self.ERPcaptureIsComplete):
            self.ERPcaptureIsComplete = True
            updateERPsignal = self.signalERP.__copy__()
            doUpdate = True
            self.signalERPavgCount += 1
            included_samples_max = self.signal_stim_preview_event_length_samples
            self.signalERPavgCountSequence[0:included_samples_max] = self.signalERPavgCountSequence[0:included_samples_max] + 1
        elif (not self.ERPcaptureIsComplete) and self.ERPcapturePrematureFired:
            self.signalERPavgCount += 1
            included_samples_max = self.signal_stim_preview_event_length_samples - self.ERP_samples_to_update
            self.signalERPavgCountSequence[0:included_samples_max] = self.signalERPavgCountSequence[0:included_samples_max] + 1
            updateERPsignal = self.signalERPpremature.__copy__()
            doUpdate = True
        if doUpdate:
            removeTail = False
            if self.signalERPavgCount > self.signalERPavgMaxCount:
                self.signalERPavgCount = self.signalERPavgMaxCount
                self.signalERPavgCountSequence[0:included_samples_max] = self.signalERPavgMaxCount
                removeTail = True
            self.signalERPavgTail.append(updateERPsignal[0:included_samples_max])
            self.signalERPsum[0:included_samples_max] += updateERPsignal[0:included_samples_max]

            if self.signalERPavgCount == 1:
                self.signalERPavg = updateERPsignal.__copy__()
            elif self.signalERPavgCount >= 2:
                if removeTail:
                    tempremovesignal = self.signalERPavgTail.popleft()
                    self.signalERPsum[0:len(tempremovesignal)] -= tempremovesignal
                self.signalERPavg = self.signalERPsum / self.signalERPavgCountSequence

            self.ERPcapturePrematureFired = False

        self.updateSendSignalView()

        if self.isStimulationTurnedOn:
            if self.isTesting():
                return self.__input_testing(eegsample_filt, emgsample_filt, eogsample_filt)
            else:
                if self.status_doTracking:

                    self.signalTrackingEEG.append(eegsample_filt)
                    self.signalTrackingEMG.append(emgsample_filt)
                    self.signalTrackingEOG.append(eogsample_filt)

                    self.checkStimulationConditions()

                    if self.status_engagedStimulation:
                        if self.isClosedLoop:
                            return self.__input_algo_ClosedLoop(eegsample_filt, emgsample_filt, eogsample_filt)
                        else:
                            return self.__input_algo_OpenLoop(eegsample_filt, emgsample_filt, eogsample_filt)

        return self.status_engagedStimulation

    def checkSampleID(self, id):
        if self.OBCI_sampleid < 0:
            self.OBCI_sampleid = id
        else:
            if self.useDaisy or self.FS_ds:
                if (id - self.OBCI_sampleid) == 2:
                    self.OBCI_sampleid = id
                elif (self.OBCI_sampleid == 255) and ((id == 0) or (id == 1)):
                    self.OBCI_sampleid = id
                else:
                    tempdiff = 0
                    if (id > self.OBCI_sampleid):
                        tempdiff = (id - self.OBCI_sampleid)
                    else:
                        tempdiff = abs((self.OBCI_sampleid - 256) - id)
                    tempdiff = tempdiff / 2
                    if (tempdiff - 1) <= 4:
                        for i in range(0, tempdiff):
                            self.input(self.dataEEG[len(self.dataEEG) - 1], self.dataEMG[len(self.dataEMG) - 1], self.dataEOG[len(self.dataEOG) - 1])
                        print('WARNING at least %d samples dropped and handled now!' % (tempdiff - 1))
                    else:
                        if not self.simulateFromCSV:
                            print('WARNING at least %d samples dropped now!' % (tempdiff - 1))
                    self.OBCI_sampleid = id
            else:
                if (id - self.OBCI_sampleid) == 1:
                    self.OBCI_sampleid = id
                elif (self.OBCI_sampleid == 255) and (id == 0):
                    self.OBCI_sampleid = id
                else:
                    tempdiff = 0
                    if (id > self.OBCI_sampleid):
                        tempdiff = (id - self.OBCI_sampleid)
                    else:
                        tempdiff = abs((self.OBCI_sampleid - 256) - id)
                    if (tempdiff - 1) <= 4:
                        for i in range(0, tempdiff):
                            self.input(self.dataEEG[len(self.dataEEG) - 1], self.dataEMG[len(self.dataEMG) - 1], self.dataEOG[len(self.dataEOG) - 1])
                        print('WARNING at least %d samples dropped and handled now!' % (tempdiff - 1))
                    else:
                        if not self.simulateFromCSV:
                            print('WARNING at least %d samples dropped now!' % (tempdiff - 1))
                    self.OBCI_sampleid = id

    def setOBCIboard(self, board):
        self.board = board
        return self.board

    def setDisplaySignalView(self, sv):
        self.sv = sv
        return self.sv

    def startStreaming(self):
        self.startTime = timeit.default_timer()
        self.EventFired.updateStartTime(self.startTime)
        if self.simulateFromCSV:
            self.csv_simulator.start_streaming(self.input_OpenBCI)
        else:
            self.board.start_streaming(self.input_OpenBCI)

    def setCSVcollect(self, csv):
        self.csv = csv
        self.EventFired.setCSVcollect(self.csv)
        return self.csv

    def correctEEGsignalPolarity(self, EEGvalues):
        tempvals = np.array(EEGvalues)
        if hasattr(EEGvalues, '__len__'):
            if len(tempvals) > 1:
                correctedValues = (self.EEG_SIGNAL_CORRECTION_FACTOR * np.array(EEGvalues))
            else:
                correctedValues = (self.EEG_SIGNAL_CORRECTION_FACTOR * np.array(EEGvalues))
        else:
            correctedValues = (self.EEG_SIGNAL_CORRECTION_FACTOR * EEGvalues)
        return correctedValues

    def timeSinceStart(self, t=None):
        if t is None:
            t = timeit.default_timer()
        if self.simulateFromCSV:
            t += self.csv_simulator.getSkipedSeconds()
        return t - self.startTime

    def changeUpdateViewInterval(self, new_updateSendOutDelaySeconds):
        self.updateViewSampleCounter = 1
        self.updateSendOutDelaySeconds = new_updateSendOutDelaySeconds
        self.updateViewSampleUpdate = int(math.ceil(self.updateSendOutDelaySeconds * self.fs))

    def clusterConsecutiveValues(self, vals, step=1):
        run = []
        res = [run]
        expect = None
        for v in vals:
            if (v == expect) or (expect is None):
                run.append(v)
            else:
                run = [v]
                res.append(run)
            expect = v + step
        return res

    def updateSendSignalView(self):
        if (self.updateViewSampleCounter % self.updateViewSampleUpdate) == 0:
            timeEndwindow = self.timeSinceStart()
            timeStartwindow = timeEndwindow - len(self.signalEEG) / self.fs
            self.signalTime = np.linspace(timeStartwindow, timeEndwindow, len(self.signalEEG))
            tempNSamples = len(self.signalEEG)
            spindles_signalTimesIndices = []

            if self.doSpindleHighlight:

                signalEEG_spindles_envelope = np.abs(hilbert(np.array(self.signalEEG_spindles)))

                ind_begin_end_threshold = np.argwhere(signalEEG_spindles_envelope > (self.spindle_amplitude_threshold_begin_end_microVolts / 2))
                ind_begin_end_threshold = ind_begin_end_threshold.flatten()
                ind_begin_end_candidates = self.clusterConsecutiveValues(ind_begin_end_threshold)

                for c in ind_begin_end_candidates:
                    if (len(c) < self.spindle_min_duration_samples) or (len(c) > self.spindle_max_duration_samples):
                        continue
                    if ((signalEEG_spindles_envelope[c] > (self.spindle_amplitude_max_microVolts / 2)).sum() > 0):
                        continue
                    if ((signalEEG_spindles_envelope[c] > (self.spindle_amplitude_threshold_detect_microVolts / 2)).sum() > 0):
                        c_adapted = np.array(c) - self.filterBP_EEG_spindle_delayInSamples
                        c_adapted = c_adapted[np.argwhere(c_adapted >= 0).flatten()]
                        spindles_signalTimesIndices.append(c_adapted)

                # instantaneous_phase = np.unwrap(np.angle(hilbert(signal)))
                # instantaneous_frequency = (np.diff(instantaneous_phase) / (2.0 * np.pi) * self.fs)

            # correct for fir fixed filter delay
            temp_signalEEG_spindles_adapted = list(self.signalEEG_spindles)[self.filterBP_EEG_spindle_delayInSamples:len(self.signalEEG_spindles)]

            temp_threshold_winsample_samplesago = None
            if self.isClosedLoop:
                temp_threshold_winsample_samplesago = [self.threshold, self.updateThresholdTimeWindowSample, (self.iSample % self.updateThresholdIntervalSample)]

            indexPlayedItem = 0
            itemsPlayed = 0
            timeSinceLastStimulusPlayedSeconds = None

            if self.isStimulationTurnedOn:
                indexPlayedItem = self.stimulusPlayer.indexPlayedItem
                itemsPlayed = self.stimulusPlayer.itemsPlayed
                timeSinceLastStimulusPlayedSeconds = self.stimulusPlayer.timeSinceLastStimulusPlayedSeconds()
                if not self.stimulusPlayer.playedAtLeastOnce:
                    timeSinceLastStimulusPlayedSeconds = None

            msg = [self.signalTime[0:tempNSamples],
                   list(self.signalEEG),
                   list(self.signalEOG),
                   list(self.signalEMG),
                   self.EventFired.getNewEvents().__copy__(),
                   self.ERPtime,
                   list(self.signalERP),
                   list(self.signalERPavg),
                   self.signalERPavgCount,
                   spindles_signalTimesIndices,
                   temp_signalEEG_spindles_adapted,
                   temp_threshold_winsample_samplesago,
                   self.threshold_EMGSignal_from_away_zero_disengage_algo,
                   indexPlayedItem,
                   itemsPlayed,
                   timeSinceLastStimulusPlayedSeconds]
            self.emit(QtCore.SIGNAL("updateSignalViewerSendMain"), msg)
            self.EventFired.resetNewEvents()
        self.updateViewSampleCounter += 1

    def input_OpenBCI(self, sample):
        # temptime = timeit.default_timer()
        if self.FS_ds and not(self.useDaisy):
            if (sample.id % 2) == 0: # discard even ids to reduce sample rate by half
                return self.status_engagedStimulation

        sample.time = self.timeSinceStart(sample.time)
        self.lastSampleTime = sample.time

        self.currentSampleID = sample.id

        if self.csv:
            self.csv.writeSample(sample)
            if self.simulateFromCSV:
                self.currentSampleWriteIndex = self.csv.getWriteIndex() + self.csv_simulator.getSkipedLines()
            else:
                self.currentSampleWriteIndex = self.csv.getWriteIndex()

        self.checkSampleID(sample.id)

        # Cz is reference
        # Fz = self.correctEEGsignalPolarity(sample.channel_data[6])
        # A1 = self.correctEEGsignalPolarity(sample.channel_data[2])
        # A2 = self.correctEEGsignalPolarity(sample.channel_data[3])

        if (self.channelEEG == 0):   # the reference channel (e.g. Cz)
            TrackingEEG = None  # to be see below.
        elif self.montage.channelNumberConnectIsBimodal(self.channelEEG):
            TrackingEEG = sample.channel_data[self.channelEEG - 1]
        else:
            TrackingEEG = self.correctEEGsignalPolarity(sample.channel_data[self.channelEEG - 1])

        if (self.channelEOG == 0):
            TrackingEOG = None  # to be see below.
        elif self.montage.channelNumberConnectIsBimodal(self.channelEOG):
            TrackingEOG = sample.channel_data[self.channelEOG - 1]
        else:
            TrackingEOG = self.correctEEGsignalPolarity(sample.channel_data[self.channelEOG - 1])

        if (self.channelEMG == 0):
            TrackingEMG = None  # to be see below.
        elif self.montage.channelNumberConnectIsBimodal(self.channelEMG):
            TrackingEMG = sample.channel_data[self.channelEMG - 1]
        else:
            TrackingEMG = self.correctEEGsignalPolarity(sample.channel_data[self.channelEMG - 1])

        if self.channelEEGrefs is not None:
            nRefs = len(self.channelEEGrefs)
            # print nRefs
            RefsAVG = np.mean(self.correctEEGsignalPolarity([sample.channel_data[i - 1] for i in self.channelEEGrefs[0:nRefs]]))
            # print RefsAVG
            if not self.montage.channelNumberConnectIsBimodal(self.channelEEG):
                if (self.channelEEG == 0):
                    TrackingEEG = RefsAVG * -1.0
                else:
                    TrackingEEG = TrackingEEG - RefsAVG

            if not self.montage.channelNumberConnectIsBimodal(self.channelEOG):
                if (self.channelEOG == 0):
                    TrackingEOG = RefsAVG * -1.0
                else:
                    TrackingEOG = TrackingEOG - RefsAVG

            if not self.montage.channelNumberConnectIsBimodal(self.channelEMG):
                if (self.channelEMG == 0):
                    TrackingEMG = RefsAVG * -1.0
                else:
                    TrackingEMG = TrackingEMG - RefsAVG


        if self.isTesting():
            TrackingEEG = self.correctEEGsignalPolarity(sample.channel_data[7 - 1])
            feedback = sample.channel_data[8 - 1]
        # print("time: %f -> sample: %10d -> read id: %3d -> channel: %s -> value: %10.2f %10.2f" % (timeit.default_timer() , self.iSample, sample.id, "TestChannel", TrackingEEG, feedback))
        # else:
        #     print("time: %f -> sample: %10d -> read id: %3d -> channels: %s %10.2f %s %10.2f %s %10.2f" % (timeit.default_timer() , self.iSample, sample.id, "EEG", TrackingEEG, "EOG", TrackingEOG, "EMG", TrackingEMG))
        res = self.input(TrackingEEG, TrackingEMG, TrackingEOG)
        # delay = timeit.default_timer() - temptime
        # if delay > 1/250.0:
        #     print "loop too slow 250 Hz: " + str(delay*1000) + " ms"
        # if delay > 1/125.0:
        #     print "loop too slow 125 Hz: " + str(delay*1000) + " ms"
        return res

    def doSimulationFromCSVFile(self, csv_simulator):
        self.simulateFromCSV = True
        self.csv_simulator = csv_simulator
        if self.simulateFromCSV:
            self.EventFired.setStartTimeOffsetSeconds(self.csv_simulator.getSkipedSeconds())
        return self.csv_simulator

    def run(self):
        if self.csv is not None:
            self.csv.activate()
            time.sleep(0.5)
            self.csv.fireStreamingStarted()
        self.startStreaming()


class StimulusEvent(object):
    def __init__(self, id, type, time=None, sampleID=None, sampleWriteIndex=None,sampleWriteIndexOffset=0):
        self.datetime = datetime.datetime.now()
        self.datetimeStamp = '%d-%d-%d_%d-%d-%d-%f' % (
        self.datetime.year, self.datetime.month, self.datetime.day, self.datetime.hour, self.datetime.minute, self.datetime.second, self.datetime.microsecond)
        self.time = time
        if time is None:
            self.time = timeit.default_timer()
        self.sampleID = sampleID
        if sampleID is None:
            self.sampleID = -1
        self.sampleWriteIndex = sampleWriteIndex
        if sampleWriteIndex is None:
            self.sampleWriteIndex = -1
        self.id = id
        self.type = type
        self.sampleWriteIndexOffset = sampleWriteIndexOffset

    def getDatetimeStamp(self):
        return self.datetimeStamp

    def getString(self):
        return self.type + "#" + str(self.id)

    def getTime(self):
        return self.time

    def getSampleID(self):
        return self.sampleID

    def getSampleWriteIndex(self):
        return self.sampleWriteIndex

    def getSampleWriteIndexOffset(self):
        return self.sampleWriteIndexOffset

class StimulusEventList(object):
    def __init__(self, csv=None, maxEventsPerType=1000000, startTimeOffsetSeconds=0):
        self.csv = csv
        self.typeLists = collections.OrderedDict()
        self.typeListsIndicesSinceReset = collections.OrderedDict()
        self.maxEventsPerType = maxEventsPerType
        self.startTime = timeit.default_timer()
        self.mixedList = collections.deque(maxlen=self.maxEventsPerType)
        self.startTimeOffsetSeconds = 0

    def setCSVcollect(self, csv):
        self.csv = csv
        return self.csv

    def updateStartTime(self, time=None):
        if time is None:
            time = timeit.default_timer()
        self.startTime = time

    def timeSinceStart(self, time=None):
        if time is None:
            time = timeit.default_timer()
        return time - self.startTime

    def setStartTimeOffsetSeconds(self,startTimeOffsetSeconds):
        self.startTimeOffsetSeconds = startTimeOffsetSeconds

    def getNewEvents(self):
        return self.mixedList

    def resetNewEvents(self):
        self.mixedList.clear()

    def appendEvent(self, type, sampleID=None, sampleWriteIndex=None, timeSinceStartOffsetSeconds=0, sampleWriteIndexOffset=0):
        tempIteratorEvent = 0
        try:
            tempIteratorEvent = len(self.typeLists[type]) + 1
        except KeyError:
            self.typeLists[type] = collections.deque(maxlen=self.maxEventsPerType)
            tempIteratorEvent = len(self.typeLists[type]) + 1
        finally:
            se = StimulusEvent(tempIteratorEvent, type, self.timeSinceStart() + timeSinceStartOffsetSeconds + self.startTimeOffsetSeconds, sampleID, sampleWriteIndex, sampleWriteIndexOffset)
            self.typeLists[type].append(se)
            self.mixedList.append(se)
            if self.csv:
                self.csv.writeStimulusEvent(se)
        return se

    def getEventTypes(self):
        self.typeLists.keys()

    def getEventListByType(self, type):
        try:
            return self.typeLists[type]
        except KeyError:
            return None


class StimulusPlayer(object):
    def __init__(self, soundBufferSize, sound_base_level_db, soundVolume, sound_rise_from_base_level_db, isClosedLoop, isSham, mainWindow, soundFrequency, playListStartIndex=0,playBackgroundNoise=False):

        self.algo = None
        self.isClosedLoop = isClosedLoop
        self.isSham = isSham
        self.soundFrequency = soundFrequency
        # reduce the buffer from 4096 to 1024 (32 does not work properly on thinkpad x220 to be below 100 ms buffer delay)?
        self.soundBufferSize = soundBufferSize
        self.soundlatency_samples = self.soundBufferSize
        self.soundlatency_seconds = self.soundlatency_samples / float(self.soundFrequency)

        self.sound_min_sound_level_reduction_db = -84

        self.preIterableTimeSeconds = 0.1

        self.firstDownToUpstateDelayInSec_list = []
        self.ThresholdDownStateDetectionPassBelow_list = []
        self.waitForFurtherDipInThreshold_list = []
        self.ThresholdUpStateDetectionPassAbove_list = []
        self.ThresholdUpStateDetectionPassBelow_list = []

        self.dialogApp = Dialogs.Dialogs()

        self.stimuliListFilePath = mainWindow.getFile("Stimuli List File", initFolder='stimulations', filterList='TXT (*.txt)')
        if self.stimuliListFilePath:
            self.stimuliListFile = open(self.stimuliListFilePath, 'r', buffering=500000)
            try:
                self.stimuliListFile = open(self.stimuliListFilePath, "r")
                reader = csv.reader(self.stimuliListFile, delimiter=',')  # creates the reader object
                self.stimuliList = collections.OrderedDict()

                for row in reader:  # iterates the rows of the file in orders
                    last_line = row
                    id = last_line[0]
                    stimFileName = last_line[1]
                    self.stimuliList[id] = stimFileName

                self.stimuliListFile.close()

            except:
                print('Stimuli List File not accessible')
                self.dialogApp.showMessageBox("Stimuli List File not accessible",
                                       "Stimuli List File " + self.stimuliListFilePath +
                                       " could not be read.\nCheck if it exists and is readable.",
                                       True, False, False)
        # time.sleep(0.5)
        self.stimuliPlayListFilePath = mainWindow.getFile("Stimuli Play List File", initFolder=os.path.dirname(os.path.abspath(str(self.stimuliListFilePath))), filterList='TXT (*.txt)')
        if self.stimuliPlayListFilePath:
            self.stimuliPlayListFile = open(self.stimuliPlayListFilePath, 'r', buffering=500000)
            try:
                self.stimuliPlayListFile = open(self.stimuliPlayListFilePath, "r")
                reader = csv.reader(self.stimuliPlayListFile, delimiter=',')  # creates the reader object
                self.stimuliPlayList = []


                for row in reader:  # iterates the rows of the file in orders
                    last_line = row
                    id = last_line[0]
                    min_pause_seconds_afterwards = float(last_line[1])
                    self.stimuliPlayList.append((id, min_pause_seconds_afterwards))
                    if (len(last_line) > 2) and  (len(last_line) <= 7) and self.isClosedLoop:
                        firstDownToUpstateDelayInSec = float(last_line[2])
                        ThresholdDownStateDetectionPassBelow = float(last_line[3])
                        waitForFurtherDipInThreshold_str = str(last_line[4])
                        waitForFurtherDipInThreshold = True
                        if waitForFurtherDipInThreshold_str == "Wait":
                            waitForFurtherDipInThreshold = True
                        else:
                            waitForFurtherDipInThreshold = False

                        ThresholdUpStateDetectionPassAbove = float(last_line[5])
                        ThresholdUpStateDetectionPassBelow = float(last_line[6])

                        self.firstDownToUpstateDelayInSec_list.append((id, firstDownToUpstateDelayInSec))
                        self.ThresholdDownStateDetectionPassBelow_list.append((id, ThresholdDownStateDetectionPassBelow))
                        self.waitForFurtherDipInThreshold_list.append((id, waitForFurtherDipInThreshold))
                        self.ThresholdUpStateDetectionPassAbove_list.append((id, ThresholdUpStateDetectionPassAbove))
                        self.ThresholdUpStateDetectionPassBelow_list.append((id, ThresholdUpStateDetectionPassBelow))


                self.stimuliPlayListFile.close()

            except:
                print('Stimuli Play List File not accessible')
                self.dialogApp.showMessageBox("Stimuli Play List File not accessible",
                                       "Stimuli List File " + self.stimuliPlayListFilePath +
                                       " could not be read.\nCheck if it exists and is readable.",
                                       True, False, False)

        self.sound_base_level_db = sound_base_level_db
        self.sound_rise_from_base_level_db = sound_rise_from_base_level_db
        self.final_sound_base_level_db_str = "minus" + str(
            abs(int(self.sound_base_level_db) + int(self.sound_rise_from_base_level_db)))

        self.playListLength = self.stimuliPlayList.__len__()
        self.prevItem = -1
        self.currentLoadedItem = playListStartIndex

        self.soundVolume = soundVolume

        self.test_stimulus_filepath = "stimuli/pinknoise/pink_noise_50_ms_44.1Hz_16bit_integer.wav"
        self.currentSoundfile = self.test_stimulus_filepath + ".44.1kHz.16bit.integer.full.db." + self.final_sound_base_level_db_str + ".db.wav"

        pygame.mixer.pre_init(frequency=self.soundFrequency, size=-16, channels=1, buffer=self.soundlatency_samples)
        pygame.mixer.init()
        # pygame.mixer.quit()
        # pygame.mixer.pre_init(frequency=self.soundFrequency, size=-16, channels=1, buffer=self.soundlatency_samples)
        # pygame.mixer.init()

        self.sound_channel = pygame.mixer.Channel(0)
        self.sound_channel.set_endevent(pygame.locals.USEREVENT)
        self.sound_channel_background = pygame.mixer.Channel(1)

        self.sound_channel.set_volume(self.soundVolume)
        self.sound_channel_background.set_volume(self.soundVolume)

        self.currentStimulusSound = None
        self.nextStimulusSound = None


        #pygame.mixer.music.set_volume(self.soundVolume)

        self.timeLastStimulusPlayed = 0
        self.currentPlayedStimWaitPlayNextStimSeconds = 0
        self.determineCurrentStimulusSoundFile()
        self.loadCurrentStimulus()
        self.playDone = True
        self.playedAtLeastOnce = False
        self.indexPlayedItem = playListStartIndex
        self.itemsPlayed = 0

        self.playBackgroundNoise = playBackgroundNoise
        self.resumeBackgroundStimulusIfFinishedwaitNotSkip = False

        self.currentBackgroundSoundfilePath = "stimuli/whitenoise/background/white_noise_10_s_44.1Hz_16bit_integer.wav"
        self.currentBackgroundSoundfile = ""
        self.determineCurrentBackgroundSoundFile()
        self.nextBackgroundStimulusSound = None
        self.loadCurrentBackgroundStimulus()

        if self.playBackgroundNoise:
            self.playCurrentBackgroundStimulus()


    def setAlgo(self,algo):
        self.algo = algo

    def determineCurrentBackgroundSoundFile(self):
        self.currentBackgroundSoundfile = self.currentBackgroundSoundfilePath + ".44.1kHz.16bit.integer.full.db." + self.final_sound_base_level_db_str + ".db.wav"

    def determineCurrentStimulusSoundFile(self):
        stimulus_id = self.stimuliPlayList[self.currentLoadedItem][0]
        stimulus_filepath = self.stimuliList[stimulus_id]
        self.currentSoundfile = stimulus_filepath + ".44.1kHz.16bit.integer.full.db." + self.final_sound_base_level_db_str + ".db.wav"

    def loadCurrentStimulus(self):
        try:
            #pygame.mixer.music.load(self.currentSoundfile)
            self.nextStimulusSound = pygame.mixer.Sound(self.currentSoundfile)
        except:
            print('ERROR loading sound file' + self.currentSoundfile + ' probably not exists?, experiment corrupted')
            self.dialogApp.showMessageBox("ERROR loading sound file",
                                     "Sound file " + self.currentSoundfile +
                                     " could not be loaded.\nCheck if it exists and is readable. PLEASE CLOSE THE APP NOW!!!",
                                     True, False, False)

    def loadCurrentBackgroundStimulus(self):
        try:
            #pygame.mixer.music.load(self.currentSoundfile)
            self.nextBackgroundStimulusSound = pygame.mixer.Sound(self.currentBackgroundSoundfile)
        except:
            print('ERROR loading background sound file' + self.currentBackgroundSoundfile + ' probably not exists?, experiment corrupted')
            self.dialogApp.showMessageBox("ERROR loading sound file",
                                     "Background Sound file " + self.currentBackgroundSoundfile +
                                     " could not be loaded.\nCheck if it exists and is readable. PLEASE CLOSE THE APP NOW!!!",
                                     True, False, False)

    def iteratePlayList(self):
        self.playDone = True
        self.prevItem = self.currentLoadedItem
        self.currentLoadedItem += 1
        if self.currentLoadedItem >= (self.playListLength):
            self.currentLoadedItem = 0
        self.determineCurrentStimulusSoundFile()
        self.loadCurrentStimulus()

    def changeSoundRiseFromBaseLeveldB(self, new_sound_rise_from_base_level_db):
        self.sound_rise_from_base_level_db = int(new_sound_rise_from_base_level_db)

        if (self.sound_base_level_db + self.sound_rise_from_base_level_db) > 0:
            self.sound_rise_from_base_level_db = -self.sound_base_level_db
        if (self.sound_base_level_db + self.sound_rise_from_base_level_db) < self.sound_min_sound_level_reduction_db:
            self.sound_rise_from_base_level_db = self.sound_min_sound_level_reduction_db - self.sound_base_level_db

        self.final_sound_base_level_db_str = "minus" + str(
            abs(int(self.sound_base_level_db) + int(self.sound_rise_from_base_level_db)))

        if self.playBackgroundNoise:
            self.determineCurrentBackgroundSoundFile()
            self.loadCurrentBackgroundStimulus()
            self.playCurrentBackgroundStimulus()

    def updateClosedLoopParameter(self):
        if self.algo and (len(self.firstDownToUpstateDelayInSec_list) > 0):
            self.algo.updateClosedLoopParameter(self.firstDownToUpstateDelayInSec_list[self.indexPlayedItem][1],
                                                self.ThresholdDownStateDetectionPassBelow_list[self.indexPlayedItem][1],
                                                self.waitForFurtherDipInThreshold_list[self.indexPlayedItem][1],
                                                self.ThresholdUpStateDetectionPassAbove_list[self.indexPlayedItem][1],
                                                self.ThresholdUpStateDetectionPassBelow_list[self.indexPlayedItem][1])


    def playCurrentBackgroundStimulus(self):
        self.sound_channel_background.play(self.nextBackgroundStimulusSound, loops=-1)

    def pauseCurrentBackgroundStimulus(self):
        self.sound_channel_background.pause()

    def resumeBackgroundStimulusIfFinishedStimulus(self,sound=None):
        if self.resumeBackgroundStimulusIfFinishedwaitNotSkip:
            self.resumeBackgroundStimulusIfFinishedStimulusWait()
        else:
            if sound is None:
                sound = self.currentStimulusSound
            self.resumeBackgroundStimulusIfFinishedStimulusSkip(sound)


    def resumeBackgroundStimulusIfFinishedStimulusWait(self):
        timer = threading.Timer(0.0, self.resumeBackgroundStimulusIfFinishedStimulusWaitHelper)
        timer.start()

    def resumeBackgroundStimulusIfFinishedStimulusWaitHelper(self):
        playing = True
        while playing:
            for event in pygame.event.get():
                if event.type is self.sound_channel.get_endevent():
                    self.sound_channel_background.unpause()
                    playing = False

    def resumeBackgroundStimulusIfFinishedStimulusSkip(self,sound):
        timer = threading.Timer(max(0.0,sound.get_length()), self.resumeBackgroundStimulusIfFinishedStimulusSkipHelper)
        timer.start()

    def resumeBackgroundStimulusIfFinishedStimulusSkipHelper(self):
        self.sound_channel_background.unpause()


    def playCurrentStimulus(self):
        if self.playBackgroundNoise and not self.isSham:
            self.pauseCurrentBackgroundStimulus()
        self.indexPlayedItem +=1
        if self.indexPlayedItem >= (self.playListLength):
            self.indexPlayedItem = 0
        if self.isClosedLoop:
            self.updateClosedLoopParameter()

        if not self.isSham:
            #pygame.mixer.music.play()
            self.currentStimulusSound = self.nextStimulusSound
            self.sound_channel.play(self.nextStimulusSound, loops=0)
            if self.playBackgroundNoise:
                self.resumeBackgroundStimulusIfFinishedStimulus()
        self.timeLastStimulusPlayed = timeit.default_timer()
        self.currentPlayedStimWaitPlayNextStimSeconds = self.stimuliPlayList[self.currentLoadedItem][1]
        self.itemsPlayed += 1


    def playTestStimulus(self):
        if self.playBackgroundNoise:
            self.pauseCurrentBackgroundStimulus()
        testStimulusSoundFile = self.test_stimulus_filepath + ".44.1kHz.16bit.integer.full.db." + self.final_sound_base_level_db_str + ".db.wav"
        test_sound = pygame.mixer.Sound(testStimulusSoundFile)
        self.sound_channel.play(test_sound, loops=0)
        if self.playBackgroundNoise:
            self.resumeBackgroundStimulusIfFinishedStimulus(sound=test_sound)

        #pygame.mixer.music.load(testStimulusSoundFile)
        #pygame.mixer.music.play()

    def timeSinceLastStimulusPlayedSeconds(self):
        return timeit.default_timer() - self.timeLastStimulusPlayed

    def isPlayable(self):
        return self.timeSinceLastStimulusPlayedSeconds() >= self.currentPlayedStimWaitPlayNextStimSeconds

    def tryIteratePlaylist(self):
        if not self.playDone:
            if self.isIterable():
                self.iteratePlayList()

    def isIterable(self):
        return (self.timeSinceLastStimulusPlayedSeconds() >= (self.currentPlayedStimWaitPlayNextStimSeconds - self.preIterableTimeSeconds)) and self.playedAtLeastOnce

    def checkAndPlayNextStimuliInPlayListWithoutIterate(self):
        isPlayableNow = self.isPlayable() and self.playDone
        if self.isPlayable() and (not self.playedAtLeastOnce):
            self.loadCurrentStimulus()
        if isPlayableNow:
            self.playCurrentStimulus()
            self.playDone = False
            if not self.playedAtLeastOnce:
                stimulus_MinDelaySeconds = self.stimuliPlayList[self.currentLoadedItem][1]
                self.currentPlayedStimWaitPlayNextStimSeconds = stimulus_MinDelaySeconds
            self.playedAtLeastOnce = True
        return isPlayableNow

    def checkAndPlayNextStimuliInPlayListWithIterate(self):
        isPlayableNow = self.isPlayable() and self.playDone
        if self.isPlayable() and (not self.playedAtLeastOnce):
            self.loadCurrentStimulus()
        if isPlayableNow:
            self.playCurrentStimulus()
            self.playDone = False
            if not self.playedAtLeastOnce:
                stimulus_MinDelaySeconds = self.stimuliPlayList[self.currentLoadedItem][1]
                self.currentPlayedStimWaitPlayNextStimSeconds = stimulus_MinDelaySeconds
            self.playedAtLeastOnce = True
        if not self.playDone:
            if self.isIterable():
                self.iteratePlayList()
        return isPlayableNow

    def getCurrentLoadedPlayStimulusID(self):
        return str(self.stimuliPlayList[self.currentLoadedItem][0])

    def getPreviousPlayedStimulusID(self):
        return str(self.stimuliPlayList[max(self.prevItem, 0)][0])

    def getCurrentPlayedStimWaitPlayNextStimSeconds(self):
        return (self.currentPlayedStimWaitPlayNextStimSeconds)

    def resetPlayList(self,playListStartIndex=0):
        self.currentLoadedItem = playListStartIndex
        self.determineCurrentStimulusSoundFile()
        #self.determineCurrentBackgroundSoundFile()
        self.loadCurrentStimulus()
        self.playDone = True
        self.indexPlayedItem = playListStartIndex


    def getIDofPlaylistbyIndex(self, idx):
        return self.stimuliPlayList[idx][0]

    def getWaitPlayNextStimSecondsOfPlaylistbyIndex(self, idx):
        return self.stimuliPlayList[idx][1]
