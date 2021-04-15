# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Settings.ui'
#
# Created by: PyQt4 UI code generator 4.12.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(956, 350)
        self.buttonBox = QtGui.QDialogButtonBox(Dialog)
        self.buttonBox.setGeometry(QtCore.QRect(720, 310, 191, 32))
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Close|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.tabWidget_main = QtGui.QTabWidget(Dialog)
        self.tabWidget_main.setEnabled(True)
        self.tabWidget_main.setGeometry(QtCore.QRect(20, 10, 911, 291))
        self.tabWidget_main.setObjectName(_fromUtf8("tabWidget_main"))
        self.tab_basic = QtGui.QWidget()
        self.tab_basic.setObjectName(_fromUtf8("tab_basic"))
        self.lineEdit_subject = QtGui.QLineEdit(self.tab_basic)
        self.lineEdit_subject.setGeometry(QtCore.QRect(10, 30, 151, 27))
        self.lineEdit_subject.setObjectName(_fromUtf8("lineEdit_subject"))
        self.label_subject = QtGui.QLabel(self.tab_basic)
        self.label_subject.setGeometry(QtCore.QRect(10, 10, 151, 17))
        self.label_subject.setObjectName(_fromUtf8("label_subject"))
        self.groupBox_channelnumber = QtGui.QGroupBox(self.tab_basic)
        self.groupBox_channelnumber.setGeometry(QtCore.QRect(10, 60, 81, 71))
        self.groupBox_channelnumber.setObjectName(_fromUtf8("groupBox_channelnumber"))
        self.radioButton_8ch = QtGui.QRadioButton(self.groupBox_channelnumber)
        self.radioButton_8ch.setGeometry(QtCore.QRect(0, 20, 61, 22))
        self.radioButton_8ch.setChecked(True)
        self.radioButton_8ch.setObjectName(_fromUtf8("radioButton_8ch"))
        self.buttonGroup_channelnumber = QtGui.QButtonGroup(Dialog)
        self.buttonGroup_channelnumber.setObjectName(_fromUtf8("buttonGroup_channelnumber"))
        self.buttonGroup_channelnumber.addButton(self.radioButton_8ch)
        self.radioButton_16ch = QtGui.QRadioButton(self.groupBox_channelnumber)
        self.radioButton_16ch.setGeometry(QtCore.QRect(0, 50, 61, 22))
        self.radioButton_16ch.setObjectName(_fromUtf8("radioButton_16ch"))
        self.buttonGroup_channelnumber.addButton(self.radioButton_16ch)
        self.checkBox_signal_viewer_full = QtGui.QCheckBox(self.tab_basic)
        self.checkBox_signal_viewer_full.setGeometry(QtCore.QRect(10, 150, 451, 22))
        self.checkBox_signal_viewer_full.setObjectName(_fromUtf8("checkBox_signal_viewer_full"))
        self.label_sampling_rate = QtGui.QLabel(self.tab_basic)
        self.label_sampling_rate.setGeometry(QtCore.QRect(80, 90, 101, 17))
        self.label_sampling_rate.setObjectName(_fromUtf8("label_sampling_rate"))
        self.comboBox_sampling_rate = QtGui.QComboBox(self.tab_basic)
        self.comboBox_sampling_rate.setEnabled(True)
        self.comboBox_sampling_rate.setGeometry(QtCore.QRect(80, 110, 111, 27))
        self.comboBox_sampling_rate.setObjectName(_fromUtf8("comboBox_sampling_rate"))
        self.comboBox_sampling_rate.addItem(_fromUtf8(""))
        self.comboBox_sampling_rate.addItem(_fromUtf8(""))
        self.comboBox_SDcardOption = QtGui.QComboBox(self.tab_basic)
        self.comboBox_SDcardOption.setGeometry(QtCore.QRect(10, 210, 181, 27))
        self.comboBox_SDcardOption.setObjectName(_fromUtf8("comboBox_SDcardOption"))
        self.label_SDcardOption = QtGui.QLabel(self.tab_basic)
        self.label_SDcardOption.setGeometry(QtCore.QRect(10, 190, 151, 17))
        self.label_SDcardOption.setObjectName(_fromUtf8("label_SDcardOption"))
        self.tabWidget_main.addTab(self.tab_basic, _fromUtf8(""))
        self.tab_advanced = QtGui.QWidget()
        self.tab_advanced.setObjectName(_fromUtf8("tab_advanced"))
        self.comboBox_logging = QtGui.QComboBox(self.tab_advanced)
        self.comboBox_logging.setGeometry(QtCore.QRect(10, 30, 181, 27))
        self.comboBox_logging.setObjectName(_fromUtf8("comboBox_logging"))
        self.comboBox_logging.addItem(_fromUtf8(""))
        self.comboBox_logging.addItem(_fromUtf8(""))
        self.label_logging = QtGui.QLabel(self.tab_advanced)
        self.label_logging.setGeometry(QtCore.QRect(10, 10, 66, 17))
        self.label_logging.setObjectName(_fromUtf8("label_logging"))
        self.lineEdit_montage_filepath = QtGui.QLineEdit(self.tab_advanced)
        self.lineEdit_montage_filepath.setGeometry(QtCore.QRect(80, 60, 681, 27))
        self.lineEdit_montage_filepath.setObjectName(_fromUtf8("lineEdit_montage_filepath"))
        self.pushButton_montage_default = QtGui.QPushButton(self.tab_advanced)
        self.pushButton_montage_default.setGeometry(QtCore.QRect(760, 60, 61, 27))
        self.pushButton_montage_default.setObjectName(_fromUtf8("pushButton_montage_default"))
        self.pushButton_montage_open = QtGui.QPushButton(self.tab_advanced)
        self.pushButton_montage_open.setGeometry(QtCore.QRect(820, 60, 51, 27))
        self.pushButton_montage_open.setObjectName(_fromUtf8("pushButton_montage_open"))
        self.label_montage = QtGui.QLabel(self.tab_advanced)
        self.label_montage.setGeometry(QtCore.QRect(10, 60, 66, 17))
        self.label_montage.setObjectName(_fromUtf8("label_montage"))
        self.tabWidget_main.addTab(self.tab_advanced, _fromUtf8(""))
        self.tab = QtGui.QWidget()
        self.tab.setObjectName(_fromUtf8("tab"))
        self.tabWidget_main.addTab(self.tab, _fromUtf8(""))

        self.retranslateUi(Dialog)
        self.tabWidget_main.setCurrentIndex(0)
        self.comboBox_sampling_rate.setCurrentIndex(0)
        self.comboBox_logging.setCurrentIndex(1)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("accepted()")), Dialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), Dialog.reject)
        QtCore.QObject.connect(self.radioButton_16ch, QtCore.SIGNAL(_fromUtf8("clicked()")), Dialog.update)
        QtCore.QObject.connect(self.radioButton_8ch, QtCore.SIGNAL(_fromUtf8("clicked()")), Dialog.update)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(_translate("Dialog", "Settings", None))
        self.lineEdit_subject.setText(_translate("Dialog", "subj1", None))
        self.label_subject.setText(_translate("Dialog", "subject name:", None))
        self.groupBox_channelnumber.setTitle(_translate("Dialog", "#channels:", None))
        self.radioButton_8ch.setText(_translate("Dialog", "8", None))
        self.radioButton_16ch.setText(_translate("Dialog", "16", None))
        self.checkBox_signal_viewer_full.setText(_translate("Dialog", "additional signal viewer of full montage (slower refresh rate)", None))
        self.label_sampling_rate.setText(_translate("Dialog", "sampling rate:", None))
        self.comboBox_sampling_rate.setItemText(0, _translate("Dialog", "250 Hz (all)", None))
        self.comboBox_sampling_rate.setItemText(1, _translate("Dialog", "125 Hz (odd)", None))
        self.label_SDcardOption.setText(_translate("Dialog", "record on SD card:", None))
        self.tabWidget_main.setTabText(self.tabWidget_main.indexOf(self.tab_basic), _translate("Dialog", "basic", None))
        self.comboBox_logging.setItemText(0, _translate("Dialog", "terminal only", None))
        self.comboBox_logging.setItemText(1, _translate("Dialog", "log file only", None))
        self.label_logging.setText(_translate("Dialog", "logging:", None))
        self.lineEdit_montage_filepath.setText(_translate("Dialog", "[DEFAULT]", None))
        self.pushButton_montage_default.setText(_translate("Dialog", "Default", None))
        self.pushButton_montage_open.setText(_translate("Dialog", "Open", None))
        self.label_montage.setText(_translate("Dialog", "montage:", None))
        self.tabWidget_main.setTabText(self.tabWidget_main.indexOf(self.tab_advanced), _translate("Dialog", "advanced", None))
        self.tabWidget_main.setTabText(self.tabWidget_main.indexOf(self.tab), _translate("Dialog", "stimulation", None))

