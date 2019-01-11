#!/bin/bash
sudo apt-get -y install qtbase5-dev-tools qtbase5-dev qt5-default qttools5-dev-tools
cd ${BASEDIR}/../software/installation/qjackctl-0.5.2
sudo apt-get -y install libjack-dev
./configure
make
sudo make install


