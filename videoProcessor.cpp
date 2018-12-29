#include "videoProcessor.h"

VideoProcessor::VideoProcessor() : delayBetweenFrames(-1), frameNumber(0), stop(false) {}

bool VideoProcessor::setInput(const std::string &fileName) {
    frameNumber = 0;
    return capture.open(fileName);
}

bool VideoProcessor::setOutput(const std::string &fileName, cv::Size frameSize, double frameRate, bool isColor) {
    outputFile = fileName;
    char c[4];
    int codec = getCodec(c);

    return writer.open(outputFile, codec, frameRate, frameSize, isColor);
}

void VideoProcessor::releaseInput() {
    capture.release();
}

void VideoProcessor::releaseOutput() {
    writer.release();
}

void VideoProcessor::setFeatureTracker(std::unique_ptr<FeatureTracker>& featureTrackerPtr) {
    featureTracker = std::move(featureTrackerPtr);
    callProcess();
}

void VideoProcessor::writeNextFrame(cv::Mat &frame) {
    writer.write(frame);
}

void VideoProcessor::callProcess() {
    callIt = true;
}

// Set a delay between each frame:
//  - 0 means wait at each frame
//  - negative means no delay
void VideoProcessor::setDelay(double delay) {
    delayBetweenFrames = int(delay);
}

cv::Size VideoProcessor::getFrameSize() {
    int w = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
    int h = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));

    return cv::Size(w, h);
}

long VideoProcessor::getFrameNumber() {
    return static_cast<long>(capture.get(cv::CAP_PROP_POS_FRAMES));
}

double VideoProcessor::getFrameRate() {
    return capture.get(cv::CAP_PROP_FPS);
}

long VideoProcessor::getTotalFrameCount() {
    return static_cast<long>(capture.get(cv::CAP_PROP_FRAME_COUNT));
}

int VideoProcessor::getCodec(char codec[4]) {
    union {
        int value;
        char code[4];
    } returned;

    returned.value = static_cast<int>(capture.get(cv::CAP_PROP_FOURCC));

    codec[0] = returned.code[0];
    codec[1] = returned.code[1];
    codec[2] = returned.code[2];
    codec[3] = returned.code[3];

    return returned.value;
}

bool VideoProcessor::setFrameNumber(long pos) {
    return capture.set(cv::CAP_PROP_POS_FRAMES, pos);
}

bool VideoProcessor::isStopped() {
    return stop;
}

bool VideoProcessor::isOpened() {
    return capture.isOpened();
}

void VideoProcessor::setStop(bool value) {
    stop = value;
}

bool VideoProcessor::run(cv::Mat &frame, cv::Mat &output, cv::Mat &mask) {
    if (!readNextFrame(frame)) {
        setStop(true);
        return false;
    }

    featureTracker->process(frame, output, mask);
    frameNumber++;
    return true;
}

bool VideoProcessor::readNextFrame(cv::Mat &frame) {
    return capture.read(frame);
}
