#include "videoFitter.h"

VideoFitter::VideoFitter() : matcher(new KeypointMatcher()) {}

void VideoFitter::process(cv::Mat &sourceFirst, cv::Mat &sourceSecond, cv::Mat &firstMask, cv::Mat &secondMask) {
    matcher->process(sourceFirst, sourceSecond, firstMask, secondMask);
    fitImages(sourceFirst, sourceSecond);
}

void VideoFitter::fitImages(cv::Mat &firstFrame, cv::Mat &secondFrame){
    std::vector<cv::DMatch> matches = matcher->getMatches();
    if (matches.empty())
        return;

    firstKeypoints = matcher->getKeypoints(true);
    secondKeypoints = matcher->getKeypoints(false);

    if (firstKeypoints.empty() || secondKeypoints.empty())
        return;

    std::sort(matches.begin(), matches.end());
    removeIncorrectMatches(matches);

    transformImages(firstFrame, secondFrame, matches);
}

void VideoFitter::transformImages(cv::Mat &firstFrame, cv::Mat &secondFrame, std::vector<cv::DMatch> matches) {
    std::vector<cv::Point2f> firstMatchedPoints, secondMatchedPoints;
    for (auto &m : matches) {
        firstMatchedPoints.push_back(firstKeypoints[unsigned(m.queryIdx)].pt);
        secondMatchedPoints.push_back(secondKeypoints[unsigned(m.trainIdx)].pt);
    }

    if (firstMatchedPoints.size() < minimalMatchedPointNumber
            || secondMatchedPoints.size() < minimalMatchedPointNumber)
        return;

    cv::Mat homographyMat = cv::findHomography(firstMatchedPoints, secondMatchedPoints, cv::RANSAC);
    if(homographyMat.empty())
        return;

    cv::warpPerspective(firstFrame, secondFrame, homographyMat, secondFrame.size(), cv::INTER_LINEAR,
                        cv::BORDER_REPLICATE);
}
