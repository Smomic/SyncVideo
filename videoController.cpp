#include "videoController.h"
#include <iostream>

VideoController::VideoController() : firstVideo(new InputProcessor()), secondVideo(new InputProcessor()), matrixSize(cv::Size(0,0)) {}

VideoController::~VideoController() {
    delete firstVideo;
    delete secondVideo;

    if (isOutputSet)
        delete outputVideo;
}

void VideoController::setFirstInput(const std::string &fileName) {
    setInput(firstVideo, fileName);
}

void VideoController::setSecondInput(const std::string &fileName) {
    setInput(secondVideo, fileName);
}

void VideoController::setInput(InputProcessor *input, const std::string &fileName) {
    if (resetInput(input, fileName))
        return;

    if (!input->set(fileName))
        emit(openingInputError(fileName));
}

bool VideoController::resetInput(InputProcessor *input, const std::string &fileName) {
    if (fileName.empty()) {
        input->release();
        return true;
    }
    return false;
}

void VideoController::setOutput(const std::string &fileName) {
    if (resetOutput(fileName))
        return;

    outputVideo = new OutputProcessor();
    InputProcessor *opened = getOpenedInput();

    if (!outputVideo->set(fileName, opened->getFrameSize(), opened->getFrameRate()))
        emit(openingOutputError(fileName));
    else
        isOutputSet = true;
}

bool VideoController::resetOutput(const std::string &filename) {
    if (filename.empty()) {
        if (isOutputSet) {
            outputVideo->release();
            isOutputSet = false;
        }
        return true;
    }
    return false;
}

InputProcessor* VideoController::getOpenedInput() {
    return (firstVideo->isOpened()) ? firstVideo : secondVideo;
}

void VideoController::run() {
    setDelay();
    setStop(false);

    cv::Mat firstOriginalFrame, secondOriginalFrame;
    cv::Mat firstOutput, secondOutput;
    cv::Mat output;
    std::unique_ptr<VideoFitter> fitter(new VideoFitter());
    std::unique_ptr<MotionFinder> firstFinder(new MotionFinder());
    std::unique_ptr<MotionFinder> secondFinder(new MotionFinder());
    cv::Mat firstMask, secondMask;

    while(isInputEnded()) {
        if (!firstVideo->run(firstOriginalFrame)
                || !secondVideo->run(secondOriginalFrame))
            break;

        processMask(firstMask);
        firstMask.copyTo(secondMask);
        firstFinder->process(firstOriginalFrame, firstOutput, firstMask);
        secondFinder->process(secondOriginalFrame, secondOutput, secondMask);
        fitter->process(firstOutput, secondOutput, firstMask, secondMask);
        blendFrames(secondOutput, secondOriginalFrame, output);

        if (isOutputSet)
            outputVideo->writeNextFrame(output);

        emit(sendFrame(output));
    }
    emit(inputEnded());
}

void VideoController::blendFrames(cv::Mat &first, cv::Mat &second, cv::Mat &output) {
    addWeighted(first, blending, second, 1 - blending, gamma, output);
}

void VideoController::setDelay() {
    firstVideo->setDelay(100. / firstVideo->getFrameRate());
    secondVideo->setDelay(100. / secondVideo->getFrameRate());
}

void VideoController::setStop(bool value) {
    firstVideo->setStop(value);
    secondVideo->setStop(value);
}

void VideoController::processMask(cv::Mat &mask) {
    mask = cv::Mat::zeros(matrixSize, CV_8U);
    cv::Rect rect;
    for (auto &m : maskRectangles) {
        rect.x = int(m->rect().left() / scaleRatio);
        rect.y = int(m->rect().top() / scaleRatio);
        rect.width = int(m->rect().width() / scaleRatio);
        rect.height = int(m->rect().height() / scaleRatio);
        rectangle(mask, rect, 255, -1);
    }
    cv::bitwise_not(mask, mask);
}

bool VideoController::isInputEnded() {
    return !firstVideo->isStopped() && !secondVideo->isStopped();
}

void VideoController::changeFirstInputPosition(int value) {
    changeInputPosition(firstVideo, value);
}

void VideoController::changeSecondInputPosition(int value) {
    changeInputPosition(secondVideo, value);
}

void VideoController::changeInputPosition(InputProcessor *input, int value) {
    long pos = input->getFrameNumber() + value;
    if (isFrameNumberCorrect(input, pos)) {
        (input == firstVideo) ? setFirstInputPosition(pos)
                              : setSecondInputPosition(pos);
    }
}

void VideoController::setFirstInputPosition(long value) {
    setInputPosition(firstVideo, value);
}

void VideoController::setSecondInputPosition(long value) {
    setInputPosition(secondVideo, value);
}

void VideoController::setInputPosition(InputProcessor *input, long value) {
    input->setFrameNumber(value);
    (input == firstVideo) ? viewActualFrame(input, secondVideo)
                          : viewActualFrame(input, firstVideo);
}

void VideoController::viewActualFrame(InputProcessor *first, InputProcessor *second) {
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
        blendFrames(output, secondOutput, output);
        setMatrixSize(output.size());
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
    InputProcessor *opened = getOpenedInput();

    if (!getOpenedVideoFrame(opened, secondFrame)) {
        emit(readingInputError(fileName));
        return false;
    }

    if (frame.size != secondFrame.size)
        return false;

    return true;
}

void VideoController::setMatrixSize(cv::Size size) {
    matrixSize = size;
}

void VideoController::setMaskRectangles(std::vector<QGraphicsRectItem *> rectangles) {
    maskRectangles = rectangles;
}

void VideoController::setScaleRatio(double ratio) {
    scaleRatio = ratio;
}
