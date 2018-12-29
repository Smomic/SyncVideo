#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include <QObject>

#include "videoProcessor.h"
#include "featureTracker.h"
#include "videoFitter.h"

class VideoController : public QObject {
    Q_OBJECT

    VideoProcessor *firstVideo;
    VideoProcessor *secondVideo;
    VideoProcessor *outputVideo;
    cv::Mat processedFrame;
    bool isOutputSet = false;
    const double blending = 0.5;

    static void setFeatureTracker(VideoProcessor *processor) {
        std::unique_ptr<FeatureTracker> tracker(new FeatureTracker());
        processor->setFeatureTracker(tracker);
    }

    static bool getOpenedVideoFrame(VideoProcessor *video, cv::Mat &output) {
        long pos = video->getFrameNumber();
        bool result = video->readNextFrame(output);
        if (!result)
            return false;

        video->setFrameNumber(pos);
        return true;
    }

    static bool isFrameNumberCorrect(VideoProcessor *video, long value) {
        return value > 0 && value <= video->getTotalFrameCount();
    }

    void setInputFile(VideoProcessor *, const std::string &);
    void setDelay();
    bool isInputEnded();
    void viewActualFrame(VideoProcessor *, VideoProcessor *);

public:
    VideoController();
    ~VideoController();
    int getNumberOfFrames(bool);
    void setFirstInputFileName(const std::string &);
    void setSecondInputFileName(const std::string &);
    void setOutputFileName(const std::string &);
    void run();
    void stop();
    void setFirstInputPosition(long);
    void setSecondInputPosition(long);
    void changeFirstInputPosition(int);
    void changeSecondInputPosition(int);
    bool checkInputFrameSize(const std::string &);

signals:
    void sendFrame(cv::Mat &);
    void inputEnded();
    void openingInputError(const std::string &);
    void openingOutputError(const std::string &);
    void readingInputError(const std::string &);
    void incorrectFrameSizeError();
};

#endif //VIDEOCONTROLLER_H
