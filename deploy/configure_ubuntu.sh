#!/bin/bash
# tested in Ubuntu 18.04 LTS
# suggested absolute minimal install
## workaround for permission of recorder_simulation to the ttyUSB*
sudo chmod 666 /dev/ttyUSB*
sudo usermod -a -G dialout $USER

# additional extension for better use of cosleep to work
sudo apt-get -y install linux-lowlatency
sudo apt-get -y install sox
sudo apt-get -yes install libsox-fmt-mp3

# recommended extensions that make the work easier on the system
sudo add-apt-repository ppa:danielrichter2007/grub-customizer
sudo apt-get update
sudo apt-get -y install grub-customizer

gsettings set org.gnome.desktop.interface clock-show-seconds true
gsettings set org.gnome.nautilus.preferences executable-text-activation ask

# optional software and setup
## for python to run and compile the packages
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


