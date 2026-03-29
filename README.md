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
In the terminal for your IDE, be sure to compile 