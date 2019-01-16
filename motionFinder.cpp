#include "motionFinder.h"

MotionFinder::MotionFinder() : imageContours(0), tracker(new OpticalFlowTracker()), contourFinder(new ContourFinder())  {}

void MotionFinder::process(cv::Mat &frame, cv::Mat &output, cv::Mat &mask) {
    frame.copyTo(output);
    findImageContours(frame);
    imageContours = contourFinder->getImageContours();
    findOpticalFlowFeatures(frame, output);
    mask &= createMask(output);
}

void MotionFinder::findOpticalFlowFeatures(cv::Mat &frame, cv::Mat &output) {
    tracker->process(frame, output);
}

void MotionFinder::findImageContours(cv::Mat &frame) {
    contourFinder->process(frame);
}

cv::Mat MotionFinder::createMask(cv::Mat &output) {
    cv::Mat mask = cv::Mat::zeros(output.size(), CV_8U);
    cv::Rect rect = getMovingObjectRectangle(output);
    rectangle(mask, rect, 255, -1);
    cv::bitwise_not(mask, mask);
    return mask;
}

cv::Rect MotionFinder::getMovingObjectRectangle(cv::Mat &output) {
    std::vector<cv::Point> maxContour = getContourWithMaximumFeatures();

    if(maxContour.empty())
        return cv::Rect();

    cv::Rect rect(boundingRect(cv::Mat(maxContour)));
   // rectangle(output, rect, cv::Scalar(0, 255, 0), 2, 8, 0);
    return rect;
}

std::vector<cv::Point> MotionFinder::getContourWithMaximumFeatures() {
    std::map<unsigned, unsigned> myMap;
    std::vector<cv::Point2f> trackedFeatures = tracker->getTrackedPoints();

    if (!trackedFeatures.empty()) {
        for (auto const &point : trackedFeatures) {
            for (unsigned i = 0; i < imageContours.size(); ++i) {
                if (cv::pointPolygonTest(imageContours[i], point, false) > 0) {
                    myMap[i]++;
                    break;
                }
            }
        }
    }
    return myMap.empty() ? std::vector<cv::Point>() : imageContours[myMap.rbegin()->first];
}
