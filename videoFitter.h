#ifndef VIDEOFITTER_H
#define VIDEOFITTER_H

#include <opencv2/core/mat.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/imgproc.hpp>
#include "keypointMatcher.h"
#include <iomanip>

class VideoFitter {

    std::unique_ptr<KeypointMatcher> matcher;
    std::vector<cv::KeyPoint> firstKeypoints;
    std::vector<cv::KeyPoint> secondKeypoints;
    const unsigned minimalMatchedPointNumber = 8;
    constexpr static const double fractionOfGoodMatches = 0.7;

    void fitImages(cv::Mat &, cv::Mat &);
public:
    VideoFitter();
    void process(cv::Mat &, cv::Mat &, cv::Mat &, cv::Mat &);

private:
    static void removeIncorrectMatches(std::vector<cv::DMatch> matches) {
        const int numGoodMatches = int(matches.size() * fractionOfGoodMatches);
        matches.erase(matches.begin() + numGoodMatches, matches.end());
    }
    void transformImages(cv::Mat &firstFrame, cv::Mat &secondFrame, std::vector<cv::DMatch> matches);
};

#endif //VIDEOFITTER_H
