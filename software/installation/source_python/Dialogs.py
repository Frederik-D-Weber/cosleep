from PyQt4 import QtGui, QtCore  # (the example applies equally well to PySide)

class Dialogs(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)

    def getFile(self,dialogName,initFolder='',filterList='all formats (*.*)'):
        # filterList = 'Images (*.png *.xpm *.jpg)'
        fname = QtGui.QFileDialog.getOpenFileName(self, dialogName,
                                                  initFolder,filterList)
        return fname

    def getInteger(self,dialogName,inputName,from_int,to_int,steps_int,default_int):
        i, okPressed = QtGui.QInputDialog.getInt(self, dialogName, inputName, default_int, from_int, to_int, steps_int)
        res = [i, okPressed]
        return res

    def getDouble(self,dialogName,inputName,from_double,to_double,decimals_int,default_double):
        d, okPressed = QtGui.QInputDialog.getDouble(self, dialogName, inputName, default_double, from_double, to_double, decimals_int)
        res = [d, okPressed]
        return res

    def getChoice(self,dialogName,inputName,items_list,current_item_int=0):
        # items = ("Red", "Blue", "Green")
        item, okPressed = QtGui.QInputDialog.getItem(self, dialogName, inputName, items_list, current_item_int, False)
        res = [item, okPressed]
        return res

    def getText(self,dialogName,inputName,inital_text):
        text, okPressed = QtGui.QInputDialog.getText(self, dialogName, inputName, QtGui.QLineEdit.Normal, inital_text)
        res = [text, okPressed]
        return res

    def showMessageBox(self, messageTitle, message, isWarning, isRichtext, hasAbortOption, isOKbuttonDefault=True):

        msgBox = QtGui.QMessageBox()
        msgBox.setWindowTitle(self.tr(messageTitle))
        if isRichtext:
            msgBox.setTextFormat(QtCore.Qt.RichText)

        if isWarning:
            msgBox.setIcon(QtGui.QMessageBox.Warning)

        msgBox.setText(message)
        if hasAbortOption:
            msgBox.setStandardButtons(QtGui.QMessageBox.Ok | QtGui.QMessageBox.Abort)
        else:
            msgBox.setStandardButtons(QtGui.QMessageBox.Ok)

        if isOKbuttonDefault:
            msgBox.setDefaultButton(QtGui.QMessageBox.Ok)
        elif hasAbortOption:
            msgBox.setDefaultButton(QtGui.QMessageBox.Abort)


        retvals = msgBox.exec_()

        if retvals == QtGui.QMessageBox.Ok:
            okPressed = True
        return okPressed