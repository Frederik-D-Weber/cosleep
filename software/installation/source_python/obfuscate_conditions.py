import sys
from Dialogs import Dialogs
import ShamCrypter
from PyQt4 import QtGui


if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)

    dialogApp = Dialogs()
    subjectConditionListFilePathUncrypted = dialogApp.getFile("Unencoded Experimental Condition File", initFolder='data/experiment', filterList='CSV and TXT (*.csv *.txt)')
    subjectConditionListFilePathEncrypted = subjectConditionListFilePathUncrypted + ".encrypted.txt"

    sc = ShamCrypter.ShamCrypter()
    sc.readSubjectConditionListWriteEcrypted(subjectConditionListFilePathUncrypted,subjectConditionListFilePathEncrypted)

    okpressed = dialogApp.showMessageBox("Finished", subjectConditionListFilePathUncrypted + "\n" + "was encoded to\n" + subjectConditionListFilePathEncrypted, True, False, True, isOKbuttonDefault=True)
    sys.exit(app.exec_())