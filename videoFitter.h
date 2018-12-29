#ifndef VIDEOFITTER_H
#define VIDEOFITTER_H

#include <opencv2/core/mat.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/imgproc.hpp>

class VideoFitter {

    cv::Mat firstFrame;
    cv::Mat secondFrame;
    cv::Mat firstMovingObjectMask;
    cv::Mat secondMovingObjectMask;
    std::vector<cv::KeyPoint> firstKeypoints;
    std::vector<cv::KeyPoint> secondKeypoints;
    std::vector<cv::DMatch> matches;
    const unsigned minimalMatchedPointNumber = 8;

    void initialize(cv::Mat &, cv::Mat &, cv::Mat &, cv::Mat &);
    cv::Mat findKeypoints(cv::Mat &, bool);
    void matchKeypoints(const cv::Mat &, const cv::Mat &);
    void fitImages();
    cv::Mat getChangedFrame();

public:
    void process(cv::Mat &, cv::Mat &, cv::Mat &, cv::Mat &);
};

#endif //VIDEOFITTER_H
