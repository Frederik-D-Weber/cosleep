#!/bin/bash
cd ${BASEDIR}/../software/installation/
git clone https://github.com/jusjusjus/edfView.git
cp ${BASEDIR}/etc/Epochsconfig.edfView.AASM.xml ${BASEDIR}/../software/installation/edfView/Epochsconfig.edfView.AASM.xml
cp ${BASEDIR}/etc/Epochsconfig.edfView.RnK.xml ${BASEDIR}/../software/installation/edfView/Epochsconfig.edfView.RnK.xml
cd edfView
sudo apt-get install qt5-default qt-qmake
export QT_SELECT=qt5
qmake
make
# copy your modified settings files, e.g. the edfView/xml_examples/ folder to your home settings file in the ~/.EDFView/ folder

