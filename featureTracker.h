/*------------------------------------------------------------------------------------------*\
This file contains material supporting chapter 13 of the book:
OpenCV3 Computer Vision Application Programming Cookbook
Third Edition
by Robert Laganiere, Packt Publishing, 2016.
\*------------------------------------------------------------------------------------------*/

#ifndef FEATURETRACKER_H
#define FEATURETRACKER_H

#include <string>
#include <vector>
#include <iomanip>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/video/tracking.hpp>

#include "motionFinder.h"

class FeatureTracker {

    cv::Mat gray;            // current gray-level image
    cv::Mat gray_prev;        // previous gray-level image
    std::vector<cv::Point2f> points[2]; // tracked features from 0->1
    std::vector<cv::Point2f> initial;   // initial position of tracked points
    std::vector<cv::Point2f> features;  // detected features
    int max_count;      // maximum number of features to detect
    double qlevel;    // quality level for feature detection
    double minDist;   // minimum distance between two feature points
    std::vector<uchar> status; // status of tracked features
    std::vector<float> err;    // error in tracking
    std::unique_ptr<MotionFinder> finder;

    void detectFeaturePoints();
    bool addNewPoints();
    bool acceptTrackedPoint(int);
    void handleTrackedPoints(cv::Mat &, cv::Mat &);

public:
    FeatureTracker();
    void process(cv::Mat &, cv::Mat &, cv::Mat &);
};

#endif //FEATURETRACKER_H
