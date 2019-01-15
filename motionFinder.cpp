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
    std::vector<cv::Rect> rectangles = getMovingRectangles(output);

    for(auto &rect : rectangles)
        rectangle(mask, rect, 255, -1);

    cv::bitwise_not(mask, mask);
    return mask;
}

std::vector<cv::Rect> MotionFinder::getMovingRectangles(cv::Mat &output) {
    std::vector<std::vector<cv::Point>> maxContours = getContoursWithFeatures();

    if(maxContours.empty())
        return std::vector<cv::Rect>();


    //rectangle(output, rect, cv::Scalar(0, 255, 0), 2, 8, 0);
    return getBoundingRectangles(maxContours);
}

std::vector<std::vector<cv::Point>> MotionFinder::getContoursWithFeatures() {
    std::map<unsigned, unsigned> myMap;
    std::vector<cv::Point2f> trackedFeatures = tracker->getTrackedPoints();
    std::vector<std::vector<cv::Point>> featureContours;

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
    return getContoursByFeaturesCount(myMap, trackedFeatures.size());
}

std::vector<cv::Rect> MotionFinder::getBoundingRectangles(std::vector<std::vector<cv::Point>> movingContours) {
    std::vector<cv::Rect> rectangles;
    for(auto &c : movingContours) {
        cv::Rect rect(boundingRect(cv::Mat(c)));
        rectangles.push_back(rect);
    }
    return rectangles;
}

std::vector<std::vector<cv::Point>> MotionFinder::getContoursByFeaturesCount(std::map<unsigned, unsigned> cMap, unsigned long countOfFeatures) {
    if (cMap.empty())
        return std::vector<std::vector<cv::Point>>();

    std::vector<std::vector<cv::Point>> movingContours;
    movingContours.push_back(imageContours[cMap.rbegin()->first]);
    unsigned featuresCount = 0;

    for(std::map<unsigned, unsigned>::reverse_iterator it=cMap.rbegin(); it != cMap.rend() && featuresCount < fractionOfMovingContours * countOfFeatures; ++it) {
            movingContours.push_back(imageContours[it->first]);
            featuresCount += it->second;
    }
    return movingContours;
}

















