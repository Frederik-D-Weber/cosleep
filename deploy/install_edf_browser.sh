#!/bin/bash
sudo apt-get -y install g++ make git-core 
#sudo apt-get -y install qtbase5-dev-tools qtbase5-dev qt5-default
sudo apt-get -y install qt4-qmake libqt4-dev
cd ${BASEDIR}/../software/installation/edfbrowser_164_source
sudo qmake-qt4
sudo make
sudo make install
