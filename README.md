# SyncVideo

## Overview
Tool for spatial synchronization of video sequences. In other words, automatically match two films with each other, 
characterized by a very similar background, and differing in the settings of the camera recording the
scene. The matching process is divided into two stages, when the first one includes the detection of a moving object 
on a given frame, while the second one involves the background synchronization of two video frames. The main issues 
are edge segmentation, optical flow and finding transformations between images using detection and matching of characteristic
points. Project made within the scope of engineering thesis.

### Screenshots
![image](https://github.com/Smomic/SyncVideo/blob/master/data/firstExample.png) <br />
![image](https://github.com/Smomic/SyncVideo/blob/master/data/secondExample.png)

## Catalogs:
- data: used video sequences examples
- src: source code of the tool

## Used software:
- g++7 (or another C++17 compilator)
- OpenCV `v3.2.0`
- Qt `v5.9`

## Required packages:
- CMake `v2.8.7 or higher`
- Git
- pkg-config


### ----------Configuration software (Ubuntu 18.04 as example)----------

- Beginning

sudo apt-get update<br />
sudo apt-get upgrade<br />
sudo apt-get install build-essential cmake unzip pkg-config<br />


- Libs for working with videos

sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev<br />
sudo apt-get install libxvidcore-dev libx264-dev<br />


- OpenCV

git clone https://github.com/opencv/opencv.git <br />
cd opencv <br />
git checkout 3.2.0 <br />
cd .. <br />
git clone https://github.com/opencv/opencv_contrib.git <br />
cd opencv_contrib <br />
git checkout 3.2.0 <br />
cd .. <br />
cd opencv <br />
mkdir build <br />
cd build <br />
cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=/usr/local \
      -D INSTALL_C_EXAMPLES=ON \
      -D INSTALL_PYTHON_EXAMPLES=OFF \
      -D WITH_TBB=ON \
      -D WITH_V4L=ON \
      -D WITH_QT=ON \
      -D WITH_OPENGL=ON \
      -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
      -D BUILD_EXAMPLES=ON ..
make <br />
sudo make install <br />
sudo ldconfig <br />


- Qt5

sudo apt-get install qt5-default
or
by installing QtCreator IDE
https://www.qt.io/download


### ---------------Installation---------------

1. cd <LOCAL_PATH>/SyncVideo/
2. mkdir build
3. cd build
4. qmake ../SyncVideo.pro
5. make
6. ./SyncVideo


