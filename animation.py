import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets

app = QtWidgets.QApplication([])
win = pg.plot([1, 3, 2, 4])
win.show()
app.exec()