#!/bin/bash
cd ../
mkdir cosleep_RAM
sudo mount -t tmpfs -o size=4000M none cosleep_RAM
rsync -av --progress ./ cosleep_RAM/
# see https://wiki.ubuntuusers.de/RAM-Disk_erstellen/
# sudo umount ${PWD}/../cosleep_RAM

