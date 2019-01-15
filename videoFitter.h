#ifndef VIDEOFITTER_H
#define VIDEOFITTER_H

#include <opencv2/core/mat.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/imgproc.hpp>
#include "keypointMatcher.h"
#include <iomanip>

class VideoFitter {

    std::unique_ptr<KeypointMatcher> matcher;
    const unsigned minimalMatchedPointNumber = 8;
    const double fractionOfGoodMatches = 0.7;

    void fitImages(cv::Mat &, cv::Mat &);
public:
    VideoFitter();
    void process(cv::Mat &, cv::Mat &, cv::Mat &, cv::Mat &);

};

#endif //VIDEOFITTER_H
