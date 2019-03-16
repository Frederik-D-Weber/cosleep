#!/bin/bash
git clone https://github.com/jusjusjus/edfView.git
cd edfView
sudo apt-get install qt5-default
export QT_SELECT=qt5
qmake
make
# copy your modified settings files, e.g. the edfView/xml_examples/ folder to your home settings file in the ~/.EDFView/ folder

