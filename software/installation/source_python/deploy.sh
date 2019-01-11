#!/bin/bash
sudo pip install --upgrade setuptools
sudo pip2 install --upgrade setuptools
sudo pip2.7 install --upgrade setuptools

#to see which are the required packages..
#sudo pip install pipreqs
#sudo pip2 install pipreqs
#sudo pip2.7 install pipreqs
# see https://stackoverflow.com/questions/35796968/get-all-modules-packages-used-by-a-python-project

#pipreqs .

sudo pip install -r requirements.txt
sudo pip2 install -r requirements.txt
sudo pip2.7 install -r requirements.txt

sudo apt-get -y install python-qt4
sudo apt-get -y install python-alsaaudio
