#!/bin/bash

CURRVER="$(python -V 2>&1)"
if !([[ "$CURRVER" =~ .*2.7.* ]]); then 
	echo "please install python 2.7"
	read -p "Press [ENTER] key to exit script..."
	exit
fi

sudo pip install pyinstaller
sudo pip2.7 install pyinstaller
sudo pip2 install pyinstaller

#sudo pip install cx_Freeze
#sudo pip2.7 install cx_Freeze
#sudo pip2 install cx_Freeze

#files must reside in a linux file system... so copy the code directory to a linux folder (not NTFS)
rm -r dist
rm -r dist_cx
pyinstaller --noconfirm --onefile hearing_threshold.py
#chmod a+x dist/hearing_threshold/hearing_threshold
#pyinstaller recording_stimulator.py
#pyinstaller --noconfirm --hidden-import scipy._lib.messagestream --onefile recording_stimulator.py
pyinstaller --noconfirm --hidden-import scipy._lib.messagestream --hidden-import vispy --hidden-import six --add-data /usr/local/lib/python2.7/dist-packages/vispy:vispy --onefile recording_stimulator.py
#cxfreeze recording_stimulator.py --target-dir dist_cx
#chmod a+x dist/recording_stimulator/recording_stimulator
pyinstaller --noconfirm --onefile obfuscate_conditions.py
mkdir rec_stim
#cp -r dist/hearing_threshold/* rec_stim/
#cp -r dist/recording_stimulator/* rec_stim/
mv dist/hearing_threshold rec_stim/hearing_threshold
mv dist/recording_stimulator rec_stim/recording_stimulator
mv dist/obfuscate_conditions rec_stim/obfuscate_conditions
#cp -r data rec_stim/
#rsync -av -f"+ */" -f"- *" "data" "rec_stim/"
mkdir rec_stim/data
mkdir rec_stim/data/rec
mkdir rec_stim/data/log
mkdir rec_stim/data/subject
mkdir rec_stim/log_terminal
cp -r data/experiment rec_stim/data/experiment
#cp -r log_terminal rec_stim/
#rsync -av -f"+ */" -f"- *" "log_terminal" "rec_stim/"
cp -r montages rec_stim/
cp -r stimulations rec_stim/
cp -r stimuli rec_stim/
rm -r ../../rec_stim
mv -f -u rec_stim ../../
# cleanup
rmdir dist
rm -r build
rm *.spec
rm *.pyc

