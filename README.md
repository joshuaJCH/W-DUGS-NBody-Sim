Requirements for PyQtGraph

Step 1:
Install Miniconda by creating an account here:
https://www.anaconda.com/download
THEN
https://www.anaconda.com/download-success

Step 2:
Ensure you have at least python 3.12 installed here:
https://www.python.org/downloads/release/python-31213/

Then, open Miniconda prompt through your terminal

Step 3:
Create an environment with this command:
conda create -n [your environment name] python=3.12

Accept the Terms of Service and proceed

Activate your environment:
conda activate [your environment name]

This will switch your current working environment to the activated one you named
Then install PyQtGraph and its dependencies:

pip install pyqtgraph
pip install PyQt6
pip install pandas

Step 4:
In your IDE, change your interpreter to the Miniconda environment that you named earlier:

For example, in VS Code:
- Press Ctrl + Shift + P (Cmd + Shift + P on MacOS)
- Under the Dropdown, click on "Python: Select Interpreter" 
- Change the interpreter path to your named environment from earlier and confirm its change
- Done!

Step 5:
In the terminal for your IDE, be sure to compile using:
g++ src/*.cpp -o [your executable name].exe

^This will compile all of the source files in the c++ project for the created executable to use

Now with the created executable, you can type out its name again in the terminal to get these available commands:
./[your executable name].exe

 Outputs:
./nbody_sim generate [body_count] [output_csv] [seed]
./nbody_sim simulate [brute_force|barnes_hut|kd_tree] [body_count] [steps] [snapshot_interval] [output_csv] [bodies_csv]
./nbody_sim benchmark [brute_force|barnes_hut|kd_tree] [steps] [output_csv]

Step 6:
Finally, we must generate the CSV values for our points and the simulation
In the same IDE terminal, we type out:

./[your executable name].exe generate [the amount of bodies you want in the simulation] [name of your output csv file].csv [unsigned int random seed]

To actually simulate the points in motion wih a chosen algorithm, we then type:
./[your executable name].exe simulate [algorithm to use: (brute_force, barnes_hut, or kd_tree)] [same bodies as before] [step count] [increment count] [output csv file name].csv [name of your output csv file].csv

Step 7:
Finally, we use the Miniconda prompt in the terminal
If not in the correct environment, you can the following command:
conda env list

Then switch to your environment using:
conda activate nbody_sim 

When in your environment, ensure you are in the correct file path as the project using:
cd [absolute path of the project]

Within the correct file path, type:
python animation.py [output csv file name].csv

The animation should open up in its separate window to visualize the Nbody Simulation at work! 
https://github.com/user-attachments/assets/76cd25aa-82e5-49f4-a1e0-e44792ae988a 

(Optional) Step 8:
To test benchmarking with different bodies and algorithms, you can use the following command in your IDE terminal:
./[your executable name].exe benchmark [algorithm to use: (brute_force, barnes_hut, or kd_tree)] [step count] [output csv file name].csv

This will test an algorithm's execution time for N bodies, 
where N begins at 10^2 bodies and ends at 10^5 bodies (10^4 for brute force)

(NOTE: it is NOT recommended to benchmark with a large step count, as this may take a long time to compute, especially for brute force)
