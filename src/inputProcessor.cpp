/*
 * Title: Spatial synchronization of video sequences
 * Author: Michał Smoła
 */

#include "inputProcessor.h"

InputProcessor::InputProcessor() : delayBetweenFrames(-1), frameNumber(0), stop(false) {}

bool InputProcessor::set(const std::string &fileName) {
    frameNumber = 0;
    return capture.open(fileName);
}

void InputProcessor::release() {
    capture.release();
}

// Set a delay between each frame:
//  - 0 means wait at each frame
//  - negative means no delay
void InputProcessor::setDelay(double delay) {
    delayBetweenFrames = int(delay);
}

cv::Size InputProcessor::getFrameSize() {
    int w = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
    int h = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));

    return cv::Size(w, h);
}

long InputProcessor::getFrameNumber() {
    return static_cast<long>(capture.get(cv::CAP_PROP_POS_FRAMES));
}

double InputProcessor::getFrameRate() {
    return capture.get(cv::CAP_PROP_FPS);
}

long InputProcessor::getTotalFrameCount() {
    return static_cast<long>(capture.get(cv::CAP_PROP_FRAME_COUNT));
}

bool InputProcessor::setFrameNumber(long pos) {
    return capture.set(cv::CAP_PROP_POS_FRAMES, pos);
}

bool InputProcessor::isStopped() {
    return stop;
}

bool InputProcessor::isOpened() {
    return capture.isOpened();
}

void InputProcessor::setStop(bool value) {
    stop = value;
}

bool InputProcessor::run(cv::Mat &frame) {
    if (!readNextFrame(frame)) {
        setStop(true);
        return false;
    }

    frameNumber++;
    return true;
}

bool InputProcessor::readNextFrame(cv::Mat &frame) {
    return capture.read(frame);
}
