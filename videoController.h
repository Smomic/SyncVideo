#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include <QObject>
#include <QGraphicsRectItem>
#include <opencv2/core/mat.hpp>
#include "inputProcessor.h"
#include "outputProcessor.h"
#include "videoFitter.h"
#include "motionFinder.h"

class VideoController : public QObject {
    Q_OBJECT

    InputProcessor *firstVideo;
    InputProcessor *secondVideo;
    OutputProcessor *outputVideo;
    std::vector<QGraphicsRectItem*> maskRectangles;
    cv::Size matrixSize;
    bool isOutputSet = false;
    double scaleRatio;
    const double blending = 0.5;
    const double gamma = 0.0;

public:
    VideoController();
    ~VideoController();
    int getNumberOfFrames(bool);
    void setFirstInput(const std::string &);
    void setSecondInput(const std::string &);
    void setOutput(const std::string &);
    void run();
    void setStop(bool);
    void setFirstInputPosition(long);
    void setSecondInputPosition(long);
    void changeFirstInputPosition(int);
    void changeSecondInputPosition(int);
    bool checkInputFrameSize(const std::string &);
    void setMaskRectangles(std::vector<QGraphicsRectItem*>);
    void setScaleRatio(double);

signals:
    void sendFrame(cv::Mat &);
    void inputEnded();
    void openingInputError(const std::string &);
    void openingOutputError(const std::string &);
    void readingInputError(const std::string &);
    void incorrectFrameSizeError();

private:
    static bool getOpenedVideoFrame(InputProcessor *video, cv::Mat &output) {
        long pos = video->getFrameNumber();
        bool result = video->readNextFrame(output);
        if (!result)
            return false;

        video->setFrameNumber(pos);
        return true;
    }

    static bool isFrameNumberCorrect(InputProcessor *video, long value) {
        return value > 0 && value <= video->getTotalFrameCount();
    }

    void setInput(InputProcessor *, const std::string &);
    void setDelay();
    bool isInputEnded();
    void viewActualFrame(InputProcessor *, InputProcessor *);
    InputProcessor* getOpenedInput();
    void changeInputPosition(InputProcessor *, int);
    void setInputPosition(InputProcessor *, long);
    bool resetInput(InputProcessor *, const std::string &);
    bool resetOutput(const std::string &);
    void blendFrames(cv::Mat &, cv::Mat &, cv::Mat &);
    void processMask(cv::Mat &);
    void setMatrixSize(cv::Size);

};

#endif //VIDEOCONTROLLER_H
