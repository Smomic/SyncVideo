QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SyncVideo
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

INCLUDEPATH += /usr/include/opencv
LIBS += -L/usr/lib -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_datasets -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_line_descriptor -lopencv_optflow -lopencv_video -lopencv_plot -lopencv_reg -lopencv_saliency -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_rgbd -lopencv_viz -lopencv_surface_matching -lopencv_text -lopencv_ximgproc -lopencv_calib3d -lopencv_features2d -lopencv_flann -lopencv_xobjdetect -lopencv_objdetect -lopencv_ml -lopencv_xphoto -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_photo -lopencv_imgproc -lopencv_core

SOURCES += \
        main.cpp \
        mainWindow.cpp \
    motionFinder.cpp \
    videoController.cpp \
    videoFitter.cpp \
    opticalFlowTracker.cpp \
    inputProcessor.cpp \
    outputProcessor.cpp \
    keypointMatcher.cpp \
    contourFinder.cpp \
    mainScene.cpp

HEADERS += \
        mainWindow.h \
    motionFinder.h \
    videoFitter.h \
    videoController.h \
    opticalFlowTracker.h \
    inputProcessor.h \
    outputProcessor.h \
    keypointMatcher.h \
    contourFinder.h \
    mainScene.h

FORMS += \
        mainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
