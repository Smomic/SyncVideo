/*
 * Title: Spatial synchronization of video sequences
 * Author: Michał Smoła
 */

#include "opticalFlowTracker.h"

OpticalFlowTracker::OpticalFlowTracker() : maxCountToDetect(10000), qualityLevel(0.1), minDistance(5.), minMovedDistance(2) {}

void OpticalFlowTracker::process(cv::Mat &frame, cv::Mat &output) {
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    frame.copyTo(output);

    if (shouldAddNewPoints())
        addNewPoints();

    copyIfFirstFrame();
    trackPoints();
    removeIncorrectTrackedPoints();
    swapPoints();

}

bool OpticalFlowTracker::shouldAddNewPoints() {
    return trackedPoints[0].size() <= unsigned(gray.size().area());
}

void OpticalFlowTracker::addNewPoints() {
    detectFeaturePoints();
    trackedPoints[0].insert(trackedPoints[0].end(), opticalFlowPoints.begin(), opticalFlowPoints.end());
    initialPosition.insert(initialPosition.end(), opticalFlowPoints.begin(), opticalFlowPoints.end());
}

void OpticalFlowTracker::detectFeaturePoints() {
    cv::goodFeaturesToTrack(gray, opticalFlowPoints, maxCountToDetect, qualityLevel, minDistance);
}

void OpticalFlowTracker::copyIfFirstFrame() {
    if (prevGray.empty())
        gray.copyTo(prevGray);
}

void OpticalFlowTracker::trackPoints() {
    cv::calcOpticalFlowPyrLK(prevGray, gray, trackedPoints[0], trackedPoints[1], pointStatus, trackingError);
}

void OpticalFlowTracker::removeIncorrectTrackedPoints() {
    int k = 0;
    for (int i = 0; i < trackedPoints[1].size(); i++) {

        if (isOpticalFlowPointCorrect(i)) {
            initialPosition[k] = initialPosition[i];
            trackedPoints[1][k++] = trackedPoints[1][i];
        }
    }
    trackedPoints[1].resize(k);
    initialPosition.resize(k);
}

bool OpticalFlowTracker::isOpticalFlowPointCorrect(int i) {

    return pointStatus[i] && isPointMoved(i);
}

bool OpticalFlowTracker::isPointMoved(int i) {
    return abs(trackedPoints[0][i].x - trackedPoints[1][i].x) +
                (abs(trackedPoints[0][i].y - trackedPoints[1][i].y)) > minMovedDistance;
}

void OpticalFlowTracker::swapPoints() {
    std::swap(trackedPoints[1], trackedPoints[0]);
    cv::swap(prevGray, gray);
}

std::vector<cv::Point2f> OpticalFlowTracker::getTrackedPoints() {
    return trackedPoints[0];
}
