#include "videoFitter.h"

VideoFitter::VideoFitter() : matcher(new KeypointMatcher()) {}

void VideoFitter::process(cv::Mat &sourceFirst, cv::Mat &sourceSecond, cv::Mat &firstMask, cv::Mat &secondMask) {
    matcher->process(sourceFirst, sourceSecond, firstMask, secondMask);
    fitImages(sourceFirst, sourceSecond);
}

void VideoFitter::fitImages(cv::Mat &firstFrame, cv::Mat &secondFrame){ //rozbić na mniejsze!!!!!!{
    std::vector<cv::DMatch> matches = matcher->getMatches();
    std::sort(matches.begin(), matches.end());

    std::vector<cv::KeyPoint> keypoints[2];
    keypoints[0] = matcher->getKeypoints(true);
    keypoints[1] = matcher->getKeypoints(false);

    if (keypoints[0].empty() || keypoints[1].empty())
        return;

    // Remove not so good matches
    const int numGoodMatches = int(matches.size() * fractionOfGoodMatches);
    matches.erase(matches.begin() + numGoodMatches, matches.end());

    cv::Mat imgMatches;
    cv::drawMatches(firstFrame, keypoints[0], secondFrame, keypoints[1], matches, imgMatches);
    //        cv::namedWindow("Matches", cv::WINDOW_NORMAL);
    //        cv::resizeWindow("Matches", 1280, 720);
    //        cv::imshow("Matches", imgMatches);

    // Extract location of good matches
    std::vector<cv::Point2f> firstMatchedPoints, secondMatchedPoints;

    for (auto &m : matches) {
        firstMatchedPoints.push_back(keypoints[0][unsigned(m.queryIdx)].pt);
        secondMatchedPoints.push_back(keypoints[1][unsigned(m.trainIdx)].pt);
    }

    if (firstMatchedPoints.size() < minimalMatchedPointNumber
            || secondMatchedPoints.size() < minimalMatchedPointNumber)
        return;

    if (matches.empty())
        return;

//    cv::Mat R = cv::estimateRigidTransform(firstMatchedPoints,secondMatchedPoints,true);

//        if(R.cols == 0)
//        {
//            return;
//        }

//        cv::Mat H = cv::Mat(3,3,R.type());
//        H.at<double>(0,0) = R.at<double>(0,0);
//        H.at<double>(0,1) = R.at<double>(0,1);
//        H.at<double>(0,2) = R.at<double>(0,2);

//        H.at<double>(1,0) = R.at<double>(1,0);
//        H.at<double>(1,1) = R.at<double>(1,1);
//        H.at<double>(1,2) = R.at<double>(1,2);

//        H.at<double>(2,0) = 0.0;
//        H.at<double>(2,1) = 0.0;
//        H.at<double>(2,2) = 1.0;


//        cv::Mat warped;
//        cv::warpPerspective(firstFrame,secondFrame,H,secondFrame.size(), cv::INTER_LINEAR,
//                                                    cv::BORDER_REPLICATE);

    cv::Mat homographyMat = cv::findHomography(firstMatchedPoints, secondMatchedPoints, cv::RANSAC);

    if(homographyMat.empty())
        return;

    cv::warpPerspective(firstFrame, secondFrame, homographyMat, secondFrame.size(), cv::INTER_LINEAR,
                        cv::BORDER_REPLICATE);
}
