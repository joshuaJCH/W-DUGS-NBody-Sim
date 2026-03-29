import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets, QtCore
import numpy as np
import pandas as pd
import os

# print("Directory:", os.getcwd())
script_dir = os.path.dirname(os.path.abspath(__file__))
file_path = os.path.join(script_dir, "simulation.csv")
sim = pd.read_csv(file_path)


# unique body ids, frames, and particle count
body_ids = sim["body_id"].unique()
frames = [group[['x','y']].values for _, group in sim.groupby("frame")]
num_bodies = len(body_ids)



# displaying nbodies in the simulation
app = QtWidgets.QApplication([])
win = pg.GraphicsLayoutWidget(show=True, title="Simulation")
bodies_label = pg.TextItem(text=f"Number of bodies: {num_bodies}", color=(255, 255, 255), anchor=(0, 1))
plot = win.addPlot(title="N-Body Simulation", labels={'left': 'Y Position', 'bottom': 'X Position'}, aspectLocked=True)
scatter = pg.ScatterPlotItem(size=2, pen=pg.mkPen(None), brush=pg.mkBrush(255, 255, 255, 120))

# add scatter and label to the plot
plot.addItem(scatter)
plot.addItem(bodies_label)


#first frame
scatter.setData(x = frames[0][:,0], y = frames[0][:,1])

#animation loop & display frames
frame_id = 0
def update():
    global frame_id
    scatter.setData(x = frames[frame_id][:,0], y = frames[frame_id][:,1])
    bodies_label.setText(f"Number of bodies: {num_bodies} | Frame: {frame_id}")

    frame_id += 1
    if frame_id >= len(frames):
        frame_id = 0 #reset to the first frame


#set the mass of the particles and color them
mass = sim.groupby("frame")["mass"].first().values
brushes = [pg.mkBrush(255, 0, 255, 255) for _ in body_ids]  # same color for all
scatter.setData(x=frames[0][:,0], y=frames[0][:,1], brush=brushes, size=3)

timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(17) #update every 17 ms for ~60 fps

#run application
win.show()
app.exec()