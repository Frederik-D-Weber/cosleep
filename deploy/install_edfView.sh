#!/bin/bash
cd ${BASEDIR}/../software/
git clone https://github.com/jusjusjus/edfView.git
cp ../deploy/etc/Epochsconfig.edfView.AASM.xml edfView/Epochsconfig.edfView.AASM.xml
cp ../deploy/etc/Epochsconfig.edfView.RnK.xml edfView/Epochsconfig.edfView.RnK.xml
cd edfView
sudo apt-get install qt5-default
export QT_SELECT=qt5
qmake
make
# copy your modified settings files, e.g. the edfView/xml_examples/ folder to your home settings file in the ~/.EDFView/ folder

