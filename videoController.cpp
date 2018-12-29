#include "videoController.h"

VideoController::VideoController() : firstVideo(new VideoProcessor()), secondVideo(new VideoProcessor()) {}

VideoController::~VideoController() {
    delete firstVideo;
    delete secondVideo;

    if (isOutputSet)
        delete outputVideo;
}

void VideoController::setFirstInputFileName(const std::string &fileName) {
    setInputFile(firstVideo, fileName);
}

void VideoController::setSecondInputFileName(const std::string &fileName) {
    setInputFile(secondVideo, fileName);
}

void VideoController::setInputFile(VideoProcessor *video, const std::string &fileName) {
    if (fileName.empty())
        video->releaseInput();
    else {
        bool result = video->setInput(fileName);
        if (!result)
            emit(openingInputError(fileName));
    }
}

void VideoController::setOutputFileName(const std::string &filename) {
    if (isOutputSet)
        return;

    outputVideo = new VideoProcessor();
    if (filename.empty()) {
        outputVideo->releaseOutput();
        return;
    }

    bool result = (firstVideo->isOpened()) ? outputVideo->setOutput(filename, firstVideo->getFrameSize(),
                                                                    firstVideo->getFrameRate())
                                           : outputVideo->setOutput(filename, secondVideo->getFrameSize(),
                                                                    secondVideo->getFrameRate());
    if (!result)
        emit(openingOutputError(filename));
    else
        isOutputSet = true;
}

void VideoController::stop() {
    firstVideo->setStop(true);
    secondVideo->setStop(true);
}

void VideoController::run() {
    setFeatureTracker(firstVideo);
    setFeatureTracker(secondVideo);
    setDelay();

    firstVideo->setStop(false);
    secondVideo->setStop(false);

    cv::Mat firstOriginalFrame, secondOriginalFrame;
    cv::Mat firstOutput, secondOutput;
    cv::Mat output;
    std::unique_ptr<VideoFitter> fitter(new VideoFitter());
    cv::Mat firstMask, secondMask;

    while(isInputEnded()) {
        if (!firstVideo->run(firstOriginalFrame, firstOutput, firstMask)
                || !secondVideo->run(secondOriginalFrame, secondOutput, secondMask))
            break;

        fitter->process(firstOutput, secondOutput, firstMask, secondMask);
        addWeighted(secondOutput, blending, secondOriginalFrame, 1 - blending, 0.0, output);

        if (isOutputSet)
            outputVideo->writeNextFrame(output);

        emit(sendFrame(output));
    }
    emit(inputEnded());
}

void VideoController::setDelay() {
    firstVideo->setDelay(100. / firstVideo->getFrameRate());
    secondVideo->setDelay(100. / secondVideo->getFrameRate());
}


bool VideoController::isInputEnded() {
    return !firstVideo->isStopped() && !secondVideo->isStopped();
}

void VideoController::changeFirstInputPosition(int value) {
    long pos = firstVideo->getFrameNumber() + value;
    if (isFrameNumberCorrect(firstVideo, pos))
        setFirstInputPosition(pos);
}

void VideoController::changeSecondInputPosition(int value) {
    long pos = secondVideo->getFrameNumber() + value;
    if (isFrameNumberCorrect(secondVideo, pos))
        setSecondInputPosition(pos);
}

void VideoController::setFirstInputPosition(long value) {
    firstVideo->setFrameNumber(value);
    viewActualFrame(firstVideo, secondVideo);
}

void VideoController::setSecondInputPosition(long value) {
    secondVideo->setFrameNumber(value);
    viewActualFrame(secondVideo, firstVideo);
}

void VideoController::viewActualFrame(VideoProcessor *first, VideoProcessor *second) {
    cv::Mat output;
    first->setFrameNumber(first->getFrameNumber() - 1);
    first->readNextFrame(output);

    if (second->isOpened()) {
        cv::Mat secondOutput;
        second->setFrameNumber(second->getFrameNumber() - 1);

        second->readNextFrame(secondOutput);
        if (output.size != secondOutput.size) {
            emit(incorrectFrameSizeError());
            return;
        }
        addWeighted(output, blending, secondOutput, 1 - blending, 0.0, output);
    }
    emit(sendFrame(output));
}

int VideoController::getNumberOfFrames(bool which) {
    return which ?  int(secondVideo->getTotalFrameCount()) : int(firstVideo->getTotalFrameCount());
}

bool VideoController::checkInputFrameSize(const std::string &fileName) {
    cv::VideoCapture capture;
    cv::Mat frame;
    bool result = capture.open(fileName);

    if (!result) {
        emit(openingInputError(fileName));
        return false;
    }

    capture.read(frame);

    cv::Mat secondFrame;
    result = (firstVideo->isOpened()) ? getOpenedVideoFrame(firstVideo, secondFrame)
                                      : getOpenedVideoFrame(secondVideo, secondFrame);

    if (!result) {
        emit(readingInputError(fileName));
        return false;
    }

    if (frame.size != secondFrame.size)
        return false;

    return true;
}
