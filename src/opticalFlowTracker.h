/*
 * Title: Spatial synchronization of video sequences
 * Author: Michał Smoła
 */

#ifndef OPTICALFLOWTRACKER_H
#define OPTICALFLOWTRACKER_H

#include <string>
#include <vector>
#include <opencv2/video/tracking.hpp>

class OpticalFlowTracker {

    cv::Mat gray;
    cv::Mat prevGray;
    std::vector<cv::Point2f> trackedPoints[2];
    std::vector<cv::Point2f> initialPosition;
    std::vector<cv::Point2f> opticalFlowPoints;
    int maxCountToDetect;
    double qualityLevel;
    double minDistance;
    double minMovedDistance;
    std::vector<uchar> pointStatus;
    std::vector<float> trackingError;

public:
    OpticalFlowTracker();
    std::vector<cv::Point2f> getTrackedPoints();
    void process(cv::Mat &, cv::Mat &);

private:
    bool shouldAddNewPoints();
    void addNewPoints();
    void trackPoints();
    void detectFeaturePoints();
    void copyIfFirstFrame();
    bool isOpticalFlowPointCorrect(int);
    void removeIncorrectTrackedPoints();
    bool isPointMoved(int);
    void swapPoints();

};

#endif //OPTICALFLOWTRACKER_H
