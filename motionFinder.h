#ifndef MOTIONFINDER_H
#define MOTIONFINDER_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opticalFlowTracker.h"
#include "contourFinder.h"
#include <algorithm>
#include <iomanip>
#include <map>

class MotionFinder {

    std::vector<std::vector<cv::Point>> imageContours;
    std::unique_ptr<OpticalFlowTracker> tracker;
    std::unique_ptr<ContourFinder> contourFinder;
    const double fractionOfMovingContours = 0.7;

public:
    MotionFinder();
    void process(cv::Mat &, cv::Mat &, cv::Mat &);

private:
    void findImageContours(cv::Mat &);
    void findOpticalFlowFeatures(cv::Mat &, cv::Mat &);
    cv::Mat createMask(cv::Mat &);
    std::vector<cv::Rect> getMovingRectangles(cv::Mat &output);
    std::vector<std::vector<cv::Point>> getContoursWithFeatures();
    std::vector<cv::Rect> getBoundingRectangles(std::vector<std::vector<cv::Point>>);
    std::vector<std::vector<cv::Point> > getContoursByFeaturesCount(std::map<unsigned, unsigned>, unsigned long);
};


#endif //MOTIONFINDER_H
