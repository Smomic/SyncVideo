/*
 * Title: Spatial synchronization of video sequences
 * Author: Michał Smoła
 */

#ifndef INPUTPROCESSOR_H
#define INPUTPROCESSOR_H

#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

class InputProcessor {

    cv::VideoCapture capture;
    int delayBetweenFrames;
    long frameNumber;
    bool stop;

public:
    InputProcessor();
    bool set(const std::string &);
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
    bool run(cv::Mat &);
    bool readNextFrame(cv::Mat &);
    void release();
};

#endif // INPUTPROCESSOR_H
