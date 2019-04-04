import csv
import numpy as np


class ChannelDefinitionsInsufficientError(Exception):
    pass


class ChannelDefinitionsInvalid(Exception):
    pass


class NumberOfChannelsOutOfRange(Exception):
    pass

class Montage():
    def __init__(self, filepath=None, nChannels=8):

        self.nChannels = nChannels
        if not ((self.nChannels == 8) or (self.nChannels == 16)):
            raise NumberOfChannelsOutOfRange

        self.filepath = filepath
        if self.filepath is None:
            if self.nChannels == 8:
                self.filepath = "montages/default.montage.8.channel.tsv"
            elif self.nChannels == 16:
                self.filepath = "montages/default.montage.16.channel.tsv"

        self.channelnumber = []
        self.label = []
        self.status = []
        self.connect = []
        self.rerefchannels = []
        self.GUI = []
        self.GUI_HP_f = []
        self.GUI_LP_f = []
        self.GUI_HP_order = []
        self.GUI_LP_order = []
        self.GUI_signalviewer_order = []
        self.referenceLine = None

        self.label_ref = ""
        self.rerefchannels_ref = ""
        self.GUI_ref = ""
        self.GUI_HP_f_ref = ""
        self.GUI_LP_f_ref = ""
        self.GUI_HP_order_ref = ""
        self.GUI_LP_order_ref = ""
        self.GUI_signalviewer_order_ref = ""

        nLinesExpected = self.nChannels + 2

        f = open(self.filepath, "r")
        reader = csv.reader(f, delimiter='\t')
        lines_read = 0
        for row in reader:
            if row[0] == "":
                continue
            lines_read += 1
            if reader.line_num == 1:
                continue
            if row[0] == "ref":
                self.label_ref = row[1]
                self.rerefchannels_ref = row[4]
                self.GUI_ref = row[5]
                self.GUI_HP_f_ref = row[6]
                self.GUI_LP_f_ref = row[7]
                self.GUI_HP_order_ref = row[8]
                self.GUI_LP_order_ref = row[9]
                self.GUI_signalviewer_order_ref = row[10]
                continue
            self.channelnumber.append(row[0])
            self.label.append(row[1])
            self.status.append(row[2])
            self.connect.append(row[3])
            self.rerefchannels.append(row[4])
            self.GUI.append(row[5])
            self.GUI_HP_f.append(row[6])
            self.GUI_LP_f.append(row[7])
            self.GUI_HP_order.append(row[8])
            self.GUI_LP_order.append(row[9])
            self.GUI_signalviewer_order.append(row[10])

        f.close()
        if lines_read < nLinesExpected:
            raise ChannelDefinitionsInsufficientError
        if not self.isValid():
            raise ChannelDefinitionsInvalid

        self.GUI_signalviewer_order_order = []
        self.GUI_signalviewer_order_channelNumbers = []

        for iChRow in range(0, len(self.GUI_signalviewer_order)):
            order = self.GUI_signalviewer_order[iChRow]
            if order == "no":
                continue
            try:
                channelNumber = int(round(float(self.channelnumber[iChRow])))
                self.GUI_signalviewer_order_order.append(float(order))
                self.GUI_signalviewer_order_channelNumbers.append(channelNumber)
            except:
                continue


        order = self.GUI_signalviewer_order_ref
        if order != "no":
            try:
                channelNumber = 0
                self.GUI_signalviewer_order_order.append(float(order))
                self.GUI_signalviewer_order_channelNumbers.append(channelNumber)
            except:
                pass

        iSorted = np.argsort(np.array(self.GUI_signalviewer_order_order))
        sGUI_signalviewer_order_channelNumbers_temp = []
        for i in iSorted:
            sGUI_signalviewer_order_channelNumbers_temp.append(self.GUI_signalviewer_order_channelNumbers[i])
        self.GUI_signalviewer_order_channelNumbers = sGUI_signalviewer_order_channelNumbers_temp

        self.rerefchannelNumbersOrderedByChannelNumber = []
        for channelNumber in range(0, self.nChannels+1):
            self.rerefchannelNumbersOrderedByChannelNumber.append(self.getRerefChannelNumbersByChannelNumber(channelNumber))

        self.rerefchannelLabelsOrderedByChannelNumber = []
        for channelNumber in range(0, self.nChannels + 1):
            label = ""
            if self.getRerefChannelNumbersByChannelNumber(channelNumber) is not None:
                skip = True
                for iChNum in self.getRerefChannelNumbersByChannelNumber(channelNumber):
                    label += ("_" if not skip else "") + self.getChannelLabelByChannelNumber(iChNum)
                    if skip:
                        skip = not skip
            self.rerefchannelLabelsOrderedByChannelNumber.append(label)


    def isValid(self):
        valid = True
        valid = valid and \
                self.label_ref != ""
        # valid = valid and \
        #         ("yes" in self.isref) or \
        #         (not ("yes" in self.isref)) and (self.isref_ref == "yes")
        valid = valid and \
                (("EEG" in self.GUI) or (self.GUI_ref == "EEG")) and \
                (("EOG" in self.GUI) or (self.GUI_ref == "EOG")) and \
                (("EMG" in self.GUI) or (self.GUI_ref == "EMG"))

        for iChNum in range(1,self.nChannels+1):
            valid = valid and \
                    (str(iChNum) in self.channelnumber)

        for iChNum in range(1,self.nChannels+1):
            valid = valid and \
                    (self.getChannelRowByChannelNumber(iChNum) is not None)
        return valid

    def getChannelDescriptionByChannelNumber(self,channelNumber,defaultRealTimeFilterOrder,wrap=False):
        return self.getChannelLabelReRefByChannelNumber(channelNumber) + \
               ("\n" if wrap else " ") + "HP " + "{:.2f}".format(round(self.getHPfilterFreqByChannelNumber(channelNumber), 2)) + \
               " o=" + \
               str(int((defaultRealTimeFilterOrder if self.getHPfilterOrderByChannelNumber(channelNumber) < 0 else self.getHPfilterOrderByChannelNumber(channelNumber)))) + \
               ("\n" if wrap else " ") + "LP " + "{:.1f}".format(round(self.getLPfilterFreqByChannelNumber(channelNumber), 1)) + \
               " o=" + \
               str(int((defaultRealTimeFilterOrder if self.getLPfilterOrderByChannelNumber(channelNumber) < 0 else self.getLPfilterOrderByChannelNumber(channelNumber))))


    def getChannelRowByChannelNumber(self, channelNumber):
        for iChRow in range(0, self.nChannels):
            if self.channelnumber[iChRow] == str(channelNumber):
                return iChRow
        return None

    def getChannelLabelByChannelNumber(self, channelNumber):
        if channelNumber == 0:
            return self.label_ref
        return self.label[self.getChannelRowByChannelNumber(channelNumber)]

    def getChannelLabelReRefByChannelNumber(self, channelNumber):
        label = self.getChannelLabelByChannelNumber(channelNumber)
        if self.rerefchannelLabelsOrderedByChannelNumber[channelNumber] == "":
            return label
        return label + "-" + self.rerefchannelLabelsOrderedByChannelNumber[channelNumber]

    def getGUIchannelLabel(self,GUI_id):
        for iChRow in range(0, self.nChannels):
            if self.GUI[iChRow] == GUI_id:
                return self.label[iChRow]
        if self.GUI_ref == GUI_id:
            return self.label_ref
        else:
            return None

    def getGUIchannelNumber(self,GUI_id):
        for iChRow in range(0, self.nChannels):
            if self.GUI[iChRow] == GUI_id:
                return iChRow+1
        if self.GUI_ref == GUI_id:
            return 0
        else:
            return None

    def getGUIchannelNumber(self,GUI_id):
        for iChRow in range(0, self.nChannels):
            if self.GUI[iChRow] == GUI_id:
                return iChRow+1
        if self.GUI_ref == GUI_id:
            return 0
        else:
            return None

    # def getRerefChannelNumbers(self):
    #     channelEEGrefs = []
    #     for iChRow in range(0, self.nChannels):
    #         if self.isref[iChRow] == "yes":
    #             channelEEGrefs.append(iChRow+1)
    #     if len(channelEEGrefs) < 1:
    #         return None
    #     return channelEEGrefs

    def getRerefChannelNumbersByChannelNumber(self,channelNumber):
        channelReRefs = None
        try:
            if channelNumber == 0:
                if self.rerefchannels_ref == "no":
                    return channelReRefs
                channelReRefs = list(map(int, self.rerefchannels_ref.split(',')))
            else:
                for iChRow in range(0, self.nChannels):
                    if self.channelnumber[iChRow] == str(channelNumber):
                        if self.rerefchannels[iChRow] == "no":
                            return channelReRefs
                        channelReRefs = list(map(int, self.rerefchannels[iChRow].split(',')))
                        break
        except:
            pass
        return channelReRefs

        for iChRow in range(0, self.nChannels):
            if self.isref[iChRow] == "yes":
                channelEEGrefs.append(iChRow+1)
        if len(channelEEGrefs) < 1:
            return None
        return channelEEGrefs

    def channelNumberConnectIsBimodal(self, channelNumber):
        if channelNumber == 0:
            return False
        return self.connect[self.getChannelRowByChannelNumber(channelNumber)] == "bimodal"

    def getByteSendStringForChannelNumber(self,channelNumber):
        if channelNumber == 1:
            return b'1'
        if channelNumber == 2:
            return b'2'
        if channelNumber == 3:
            return b'3'
        if channelNumber == 4:
            return b'4'
        if channelNumber == 5:
            return b'5'
        if channelNumber == 6:
            return b'6'
        if channelNumber == 7:
            return b'7'
        if channelNumber == 8:
            return b'8'
        if channelNumber == 9:
            return b'Q'
        if channelNumber == 10:
            return b'W'
        if channelNumber == 11:
            return b'E'
        if channelNumber == 12:
            return b'R'
        if channelNumber == 13:
            return b'T'
        if channelNumber == 14:
            return b'Y'
        if channelNumber == 15:
            return b'U'
        if channelNumber == 16:
            return b'I'

    def getFilterValueByChannelNumber(self,filterRows,filterRef,channelNumber):
        value = -1
        try:
            if channelNumber == 0:
                return float(filterRef)
            for iChRow in range(0, self.nChannels):
                if self.channelnumber[iChRow] == str(channelNumber):
                    return float(filterRows[iChRow])
        except:
            pass
        return value

    def getHPfilterFreqByChannelNumber(self,channelNumber):
        return self.getFilterValueByChannelNumber(self.GUI_HP_f,self.GUI_HP_f_ref,channelNumber)

    def getLPfilterFreqByChannelNumber(self, channelNumber):
        return self.getFilterValueByChannelNumber(self.GUI_LP_f,self.GUI_LP_f_ref,channelNumber)

    def getHPfilterOrderByChannelNumber(self, channelNumber):
        return self.getFilterValueByChannelNumber(self.GUI_HP_order,self.GUI_HP_order_ref,channelNumber)

    def getLPfilterOrderByChannelNumber(self, channelNumber):
        return self.getFilterValueByChannelNumber(self.GUI_LP_order,self.GUI_LP_order_ref,channelNumber)

    def getOpenBCIChannelSetupSendStringByChannelNumber(self,channelNumber):
        # start of channel settings
        # channel selection
        # if turned off (1) or on (0)
        # BIAS_SET included, 0 = Remove form BIAS, 1 = Include in BIAS (default)
        # gain is 24 always
        # INPUT_TYPE_SET
        # SRB2 connect, 0 = Disconnect this input from SRB2, 1 = Connect this input to SRB2 (default)
        # SRB1 connect, 0 = Disconnect all N inputs from SRB1 (default), 1 = Connect all N inputs to SRB1
        # end of channel settings
        sendByteString = b'x' + \
        self.getByteSendStringForChannelNumber(channelNumber) + \
        (b'0' if self.status[self.getChannelRowByChannelNumber(channelNumber)] == "on" else b'1') + \
        b'6' + \
        b'0' + \
        (b'0' if self.channelNumberConnectIsBimodal(channelNumber) else b'1') + \
        (b'0' if self.channelNumberConnectIsBimodal(channelNumber) else b'1') + \
        b'0' + \
        b'X'
        return sendByteString

