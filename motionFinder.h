#ifndef MOTIONFINDER_H
#define MOTIONFINDER_H

#include "opticalFlowTracker.h"
#include "contourFinder.h"
#include <algorithm>
#include <iomanip>
#include <map>

class MotionFinder {

    std::vector<std::vector<cv::Point>> imageContours;
    std::unique_ptr<OpticalFlowTracker> tracker;
    std::unique_ptr<ContourFinder> contourFinder;

public:
    MotionFinder();
    void process(cv::Mat &, cv::Mat &, cv::Mat &);

private:
    void findImageContours(cv::Mat &);
    void findOpticalFlowFeatures(cv::Mat &, cv::Mat &);
    cv::Mat createMask(cv::Mat &);
    cv::Rect getMovingObjectRectangle(cv::Mat &output);
    std::vector<cv::Point> getContourWithMaximumFeatures();
};


#endif //MOTIONFINDER_H
