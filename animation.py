import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore
import numpy as np
import pandas as pd 
import os

print("Directory:", os.getcwd())
script_dir = os.path.dirname(os.path.abspath(__file__))
file_path = os.path.join(script_dir, "simulation.csv")
sim = pd.read_csv(file_path)


# unique body ids
body_ids = sim["body_id"].unique()

frames = [group[['x','y']].values for _, group in sim.groupby("frame")]

# displaying nbodies in the simulation
app = QtWidgets.QApplication([])
win = pg.GraphicsLayoutWidget(show=True, title="Simulation")
plot = win.addPlot(title="N-Body Simulation")
scatter = pg.ScatterPlotItem(size=100, pen=pg.mkPen(None), brush=pg.mkBrush(255, 255, 255, 120))
plot.addItem(scatter)

#first frame
scatter.setData(x = frames[0][:,0], y = frames[0][:,1])

#animation loop
frame_id = 0
def update():
    global frame_id
    scatter.setData(x = frames[frame_id][:,0], y = frames[frame_id][:,1])
    frame_id += 1
    if frame_id >= len(frames):
        frame_id = 0 #reset to the first frame

mass = sim.groupby("frame")["mass"].first().values
brushes = [pg.mkBrush(255, 0, 255, 255) for _ in body_ids]  # same color for all
scatter.setData(x=frames[0][:,0], y=frames[0][:,1], brush=brushes, size=3)

timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(17) #update every 17 ms for ~60 fps

#run application
win.show()
app.exec()