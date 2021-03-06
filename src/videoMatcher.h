/*
 * Title: Spatial synchronization of video sequences
 * Author: Michał Smoła
 */

#ifndef VIDEOFITTER_H
#define VIDEOFITTER_H

#include "keypointMatcher.h"
#include <iomanip>

class VideoMatcher {

    std::unique_ptr<KeypointMatcher> matcher;
    std::vector<cv::KeyPoint> firstKeypoints;
    std::vector<cv::KeyPoint> secondKeypoints;
    const unsigned minimalMatchedPointNumber = 8;
    constexpr static const double fractionOfGoodMatches = 0.7;

    void matchImages(cv::Mat &, cv::Mat &);

public:
    VideoMatcher();
    void process(cv::Mat &, cv::Mat &, cv::Mat &, cv::Mat &);

private:
    static void removeIncorrectMatches(std::vector<cv::DMatch> matches) {
        const int numGoodMatches = int(matches.size() * fractionOfGoodMatches);
        matches.erase(matches.begin() + numGoodMatches, matches.end());
    }
    void transformImages(cv::Mat &, cv::Mat &, std::vector<cv::DMatch>);
};

#endif //VIDEOFITTER_H
