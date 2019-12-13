#!/bin/bash
# tested in Ubuntu 18.04 LTS
# required setup for COsleep to work
# it is recommended to have the OpenBCI dongle plugged in while running

## workaround for permission of recording_simulator to the ttyUSB* in UBUNTU
#sudo chmod 666 /dev/ttyUSB*
#sudo usermod -a -G dialout $USER
# this might take a restart of the system so, restart the system
# check if the user access to /dev/ttyUSB* worked by entering..
# groups
# and see if dialout is included in the printed list
# also check if dialout has access to /dev/ttyUSB* by entering...
# ls -lh /dev/ttyUSB*
# you should unplug and replug the dongle several times, in several USB-ports and run the command
# sudo chmod 666 /dev/ttyUSB*
# this assures you have write access to /dev/ttyUSB0 and /dev/ttyUSB1 and ...
# but this allows any user to access serial port, ... which in most use cases is probably fine, but be aware.
# If this still does not work, and you do not run Ubuntu as your linux but another debian derived distribution then try
# sudo gedit /etc/udev/rules.d/50-ttyusb.rules
# and paste the following line in there WITHOUT the leading hash#, then save the changes and restart the OS
# echo "KERNEL==\"ttyUSB[0-9]*\",MODE=\"0666\""
# or trust to following command to be run properly
echo "KERNEL==\"ttyUSB[0-9]*\",MODE=\"0666\"" | sudo tee /etc/udev/rules.d/50-ttyusb.rules

## to have near-realtime responses and stricter time management
sudo apt-get update
sudo apt-get -y install linux-lowlatency

## to create audio files for stimulation you need sox
sudo apt-get -y install sox
## and if you want to work with mp3-based files you need this for sox
sudo apt-get -y install libsox-fmt-mp3

## recommended setup to make selection of the low-latency kernel easier, and if you run other OS in parallel
sudo add-apt-repository ppa:danielrichter2007/grub-customizer
sudo apt-get update
sudo apt-get -y install grub-customizer

sudo apt-get -y install libcanberra-gtk*
sudo apt-get -y install python-qt4-gl
sudo apt-get -y install libqt4-opengl
sudo apt-get -y install python-opengl


## show the clock with seconds in gnome desktops
gsettings set org.gnome.desktop.interface clock-show-seconds true

## make files executable able to execute by double clicking and being asked if you want to run it
gsettings set org.gnome.nautilus.preferences executable-text-activation ask

# optional software and setup
## for python to run and compile and recompile the packages
sudo apt-get -y install cmake
sudo apt-get -y install python # probably already up to date
sudo apt-get -y install python-pip
sudo apt-get -y install python-qt4
sudo apt-get -y install python-alsaaudio

## to run from Files by double clicking
sudo apt-get -y install nautilus-actions # might not work

## to configure and check sound card properties
sudo apt-get -y install jackd2

## to track the CPU usage in a GUI
sudo add-apt-repository ppa:cpug-devs/ppa
sudo apt-get update
sudo apt-get -y install cpu-g # might not work

sudo add-apt-repository universe
sudo apt-get update
sudo apt-get -y install exfat-fuse exfat-utils

echo ""
echo ""
echo "Restart your computer For changes to take effect"


