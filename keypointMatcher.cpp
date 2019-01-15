#include "keypointMatcher.h"

void KeypointMatcher::process(cv::Mat &sourceFirst, cv::Mat &sourceSecond, cv::Mat &firstMask, cv::Mat &secondMask) {
    initialize(sourceFirst, sourceSecond, firstMask, secondMask);
    cv::Mat firstGrey, secondGrey;
    cv::cvtColor(firstFrame, firstGrey, cv::COLOR_BGR2GRAY);
    cv::cvtColor(secondFrame, secondGrey, cv::COLOR_BGR2GRAY);
    matchKeypoints(findKeypoints(firstGrey, true), findKeypoints(secondGrey, false));
}

void KeypointMatcher::initialize(cv::Mat &first, cv::Mat &second, cv::Mat &firstMask, cv::Mat &secondMask) {
    firstFrame = first;
    secondFrame = second;
    firstMovingObjectMask = firstMask;
    secondMovingObjectMask = secondMask;
}

cv::Mat KeypointMatcher::findKeypoints(cv::Mat &source, bool flag) {
    cv::Ptr<cv::Feature2D> orb = cv::ORB::create();
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    flag ?  orb->detectAndCompute(source, firstMovingObjectMask, firstKeypoints, descriptors) :
            orb->detectAndCompute(source, secondMovingObjectMask, secondKeypoints, descriptors);

    return descriptors;
}

bool KeypointMatcher::matchKeypoints(const cv::Mat &queryDescriptors, const cv::Mat &trainDescriptors) {
    if (queryDescriptors.empty() || trainDescriptors.empty())
        return false;

    cv::BFMatcher matcher(cv::NORM_HAMMING, true);
    matcher.match(queryDescriptors, trainDescriptors, matches);
    return true;
}

std::vector<cv::DMatch> KeypointMatcher::getMatches() {
    return matches;
}

std::vector<cv::KeyPoint> KeypointMatcher::getKeypoints(bool which) {
    return (which) ? firstKeypoints : secondKeypoints;
}
