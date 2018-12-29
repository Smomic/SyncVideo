/*------------------------------------------------------------------------------------------*\
This file contains material supporting chapter 12 of the book:
OpenCV3 Computer Vision Application Programming Cookbook
Third Edition
by Robert Laganiere, Packt Publishing, 2016.
\*------------------------------------------------------------------------------------------*/

#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QObject>
#include <iomanip>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "featureTracker.h"

class VideoProcessor : public QObject {
    Q_OBJECT

    cv::VideoCapture capture;
    std::unique_ptr<FeatureTracker> featureTracker;
    cv::VideoWriter writer;
    std::string outputFile;
    int delayBetweenFrames;
    long frameNumber;
    bool stop;
    bool callIt;

    void callProcess();
    int getCodec(char [4]);

public:
    VideoProcessor();
    bool setInput(const std::string &);
    bool setOutput(const std::string &, cv::Size, double, bool = true);
    void setFeatureTracker(std::unique_ptr<FeatureTracker>&);
    void writeNextFrame(cv::Mat &);
    void stopAtActualFrame(long);
    void setDelay(double);
    cv::Size getFrameSize();
    long getFrameNumber();
    double getFrameRate();
    long getTotalFrameCount();
    bool setFrameNumber(long);
    bool isStopped();
    bool isOpened();
    void setStop(bool);
    bool run(cv::Mat &, cv::Mat &, cv::Mat &);
    bool readNextFrame(cv::Mat &);
    void releaseInput();
    void releaseOutput();
};

#endif // VIDEOPROCESSOR_H
