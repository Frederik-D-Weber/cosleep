import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui
import numpy as np
from pyqtgraph.dockarea import *
import sys

app = QtGui.QApplication(sys.argv)

win = QtGui.QMainWindow()
area = DockArea()
win.setCentralWidget(area)
win.resize(1000,500)
win.setWindowTitle('pyqtgraph example: dockarea')


d1 = Dock("Dock1")
d2 = Dock("Dock2")
area.addDock(d1, 'bottom')
area.addDock(d2, 'bottom', d1)

w1 = pg.PlotWidget(title="Dock 1 plot")
w1.plot(np.random.normal(size=100)*1e12)
w1.plotItem.showGrid(x=True,y=True,alpha=1)
d1.addWidget(w1)

w2 = pg.PlotWidget(title="Dock 2 plot")
w2.plot(np.random.normal(size=100))
w2.plotItem.showGrid(x=True,y=True,alpha=1)
w2.setXLink(w1)
d2.addWidget(w2)

win.show()

sys.exit(app.exec_())