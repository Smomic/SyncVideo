#include "videoFitter.h"

void VideoFitter::process(cv::Mat &sourceFirst, cv::Mat &sourceSecond, cv::Mat &firstMask, cv::Mat &secondMask) {
    initialize(sourceFirst, sourceSecond, firstMask, secondMask);
    cv::Mat firstGrey, secondGrey;
    cv::cvtColor(firstFrame, firstGrey, cv::COLOR_BGR2GRAY);
    cv::cvtColor(secondFrame, secondGrey, cv::COLOR_BGR2GRAY);
    matchKeypoints(findKeypoints(firstGrey, true), findKeypoints(secondGrey, false));
    fitImages();
}

void VideoFitter::initialize(cv::Mat &first, cv::Mat &second, cv::Mat &firstMask, cv::Mat &secondMask) {
    firstFrame = first;
    secondFrame = second;
    firstMovingObjectMask = firstMask;
    secondMovingObjectMask = secondMask;
}

cv::Mat VideoFitter::findKeypoints(cv::Mat &source, bool flag) {
    cv::Ptr<cv::Feature2D> orb = cv::ORB::create();
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;

    flag ?  orb->detectAndCompute(source, firstMovingObjectMask, firstKeypoints, descriptors) :
            orb->detectAndCompute(source, secondMovingObjectMask, secondKeypoints, descriptors);

    return descriptors;
}

void VideoFitter::matchKeypoints(const cv::Mat &queryDescriptors, const cv::Mat &trainDescriptors) {
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
    std::vector<cv::Mat> masks = {firstMovingObjectMask, secondMovingObjectMask};
    matcher->match(queryDescriptors, trainDescriptors, matches);
}

void VideoFitter::fitImages() {
    std::sort(matches.begin(), matches.end());

    // Remove not so good matches
    const int numGoodMatches = int(matches.size() * 0.5);
    matches.erase(matches.begin() + numGoodMatches, matches.end());

    cv::Mat imgMatches;
    cv::drawMatches(firstFrame, firstKeypoints, secondFrame, secondKeypoints, matches, imgMatches);
    //        cv::namedWindow("Matches", cv::WINDOW_NORMAL);
    //        cv::resizeWindow("Matches", 1280, 720);
    //        cv::imshow("Matches", imgMatches);

    // Extract location of good matches
    std::vector<cv::Point2f> firstMatchedPoints, secondMatchedPoints;

    for (auto &m : matches) {
        firstMatchedPoints.push_back(firstKeypoints[unsigned(m.queryIdx)].pt);
        secondMatchedPoints.push_back(secondKeypoints[unsigned(m.trainIdx)].pt);
    }

    if (firstMatchedPoints.size() < minimalMatchedPointNumber
            || secondMatchedPoints.size() < minimalMatchedPointNumber)
        return;

    cv::Mat homographyMat = cv::findHomography(firstMatchedPoints, secondMatchedPoints, cv::RANSAC);
    cv::warpPerspective(firstFrame, secondFrame, homographyMat, secondFrame.size(), cv::INTER_LINEAR,
                        cv::BORDER_REPLICATE);
}
