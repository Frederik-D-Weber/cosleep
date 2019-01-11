
Requirements
============
Qt  http://www.qt.io/

Minimum version 4.7.1 or later, preferable 4.8.7

If you insist on using Qt5 (which I don't recommend), it must be version >= 5.9.1.
For info how to compile Qt5 from source, scroll down.


The GCC compiler on Linux or Mingw-w64 on windows. <http://mingw-w64.sourceforge.net/>

http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/4.8.2/threads-posix/dwarf/i686-4.8.2-release-posix-dwarf-rt_v3-rev3.7z/download

Do not use microsoft tools or compilers.



Introduction
============

EDFbrowser is a Qt4 application and uses qmake as part of the build
process.  qmake is part of your Qt4 installation.



Build and run without "installing"
==================================

You need to have Qt and GCC (Mingw-w64 on Windows) installed.

Extract the sourcefile and enter the following commands:

qmake

make

Now you can run the program.



Installing
==========

sudo make install



How to compile
==============

- Install Git, the GCC compiler and the Qt4 development package:
  Linux Mint 18: sudo apt-get install git-core libqt4-dev libqt4-core g++
  openSuse Leap 42.2: sudo zypper in git-core libqt4-devel
  Fedora 25: sudo dnf install git-core qt-devel gcc-c++
  Ubuntu 18.04: sudo apt-get install g++ make git-core qtbase5-dev-tools qtbase5-dev qt5-default

- Download the source code:
  mkdir EDFbrowser_git
  cd EDFbrowser_git
  git init
  git pull https://github.com/Teuniz/EDFbrowser

- Create the makefile:
  Linux Mint 18: qmake
  openSuse Leap 42.2: qmake
  Fedora 25: qmake-qt4
  Ubuntu: qmake

- Compile the source code and install it:
  make -j8
  sudo make install

Now you can run the program by typing: edfbrowser


In case you insist on using Qt5, the recommended way is not to use the Qt5 libraries that comes with your distro.
Instead, download and compile the Qt5 source and use that to compile EDFbrowser.
Here's a step by step list with instructions:

First, fulfill the requirements for Qt:

http://doc.qt.io/qt-5/linux.html

Debian/Ubuntu: sudo apt-get install build-essential libgl1-mesa-dev libcups2-dev libx11-dev

Fedora: sudo dnf groupinstall "C Development Tools and Libraries"
        sudo dnf install mesa-libGL-devel cups-devel libx11-dev

openSUSE: sudo zypper install -t pattern devel_basis
          sudo zypper install xorg-x11-devel cups-devel freetype-devel fontconfig-devel

#############################################################################################
#                                                                                           #
# Compile a static version of the Qt5 libraries excluding all modules that are not needed.  #
#                                                                                           #
# This will not mess with your system libraries. The new compiled libraries will be stored  #
#                                                                                           #
# in a new and separate directory: /usr/local/Qt-5.9.5                                      #
#                                                                                           #
# It will not interfere with other Qt programs.                                             #
#                                                                                           #
#############################################################################################

mkdir Qt5-source

cd Qt5-source

wget http://ftp1.nluug.nl/languages/qt/official_releases/qt/5.9/5.9.5/single/qt-everywhere-opensource-src-5.9.5.tar.xz

here is a list of download mirrors: https://download.qt.io/static/mirrorlist/
The Qt source package you are going to need is: qt-everywhere-opensource-src-5.9.5.tar.xz

tar -xvf qt-everywhere-opensource-src-5.9.5.tar.xz

cd qt-everywhere-opensource-src-5.9.5

./configure -v -release -opensource -confirm-license -c++std c++11 -static -accessibility -fontconfig -skip qtdeclarative -skip qtconnectivity -skip qtmultimedia -no-qml-debug -qt-zlib -no-mtdev -no-journald -qt-libpng -qt-libjpeg -system-freetype -qt-harfbuzz -no-openssl -no-libproxy -no-glib -nomake examples -nomake tests -no-compile-examples -cups -no-evdev -no-dbus -no-eglfs -qreal double -no-opengl -skip qtlocation -skip qtsensors -skip qtwayland -skip qtgamepad -skip qtserialbus

(takes about 1.5 minutes)

make -j8

(takes about 11 minutes)

sudo make install

Now go to the directory that contains the EDFbrowser sourcecode and enter the following commands:

/usr/local/Qt-5.9.5/bin/qmake

make -j8

sudo make install

Now you can run the program by typing: edfbrowser

Congratulations!
You have compiled a static version of EDFbrowser that can be deployed on other systems without the need
to install the Qt libraries.
In order to reduce the size of the executable, run the following commands:

strip -s edfbrowser

upx edfbrowser



