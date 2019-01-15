#ifndef KEYPOINTMATCHER_H
#define KEYPOINTMATCHER_H


#include <opencv2/core/mat.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/imgproc.hpp>

class KeypointMatcher {

    cv::Mat firstFrame;
    cv::Mat secondFrame;
    cv::Mat firstMovingObjectMask;
    cv::Mat secondMovingObjectMask;
    std::vector<cv::KeyPoint> firstKeypoints;
    std::vector<cv::KeyPoint> secondKeypoints;
    std::vector<cv::DMatch> matches;

public:
    void process(cv::Mat &, cv::Mat &, cv::Mat &, cv::Mat &);
    std::vector<cv::DMatch> getMatches();
    std::vector<cv::KeyPoint> getKeypoints(bool);

private:
    void initialize(cv::Mat &, cv::Mat &, cv::Mat &, cv::Mat &);
    cv::Mat findKeypoints(cv::Mat &, bool);
    bool matchKeypoints(const cv::Mat &, const cv::Mat &);
};

#endif // KEYPOINTMATCHER_H
