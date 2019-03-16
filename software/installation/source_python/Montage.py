import csv

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
        self.isref = []
        self.GUI = []
        self.referenceLine = None

        self.label_ref = ""
        self.isref_ref = ""
        self.GUI_ref = ""

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
                self.isref_ref = row[4]
                self.GUI_ref = row[5]
                continue
            self.channelnumber.append(row[0])
            self.label.append(row[1])
            self.status.append(row[2])
            self.connect.append(row[3])
            self.isref.append(row[4])
            self.GUI.append(row[5])
        f.close()
        if lines_read < nLinesExpected:
            raise ChannelDefinitionsInsufficientError
        if not self.isValid():
            raise ChannelDefinitionsInvalid

    def isValid(self):
        valid = True
        valid = valid and \
                self.label_ref != ""
        valid = valid and \
                ("yes" in self.isref) or \
                (not ("yes" in self.isref)) and (self.isref_ref == "yes")
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

    def getChannelRowByChannelNumber(self, channelNumber):
        for iChRow in range(0, self.nChannels):
            if self.channelnumber[iChRow] == str(channelNumber):
                return iChRow
        return None

    def getChannelLabelByChannelNumber(self, channelNumber):
        if channelNumber == 0:
            return self.label_ref
        return self.label[self.getChannelRowByChannelNumber(channelNumber)]

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

    def getRerefChannelNumbers(self):
        channelEEGrefs = []
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

