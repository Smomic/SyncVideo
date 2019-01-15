#include "opticalFlowTracker.h"

OpticalFlowTracker::OpticalFlowTracker() : maxCountToDetect(10000), qualityLevel(0.1), minDistance(5.) {}

void OpticalFlowTracker::process(cv::Mat &frame, cv::Mat &output) {
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    frame.copyTo(output);

    if (shouldAddNewPoints())
        addNewPoints();

    copyIfFirstFrame();
    trackPoints();
    removeIncorrectTrackedPoints();
   // handleTrackedPoints(frame, output);
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
    cv::goodFeaturesToTrack(gray, // the image
                            opticalFlowPoints,   // the output detected features
                            maxCountToDetect,  // the maximum number of features
                            qualityLevel,     // quality level
                            minDistance);   // min distance between two features
}

void OpticalFlowTracker::copyIfFirstFrame() {
    if (prevGray.empty())
        gray.copyTo(prevGray);
}

void OpticalFlowTracker::trackPoints() {
    cv::calcOpticalFlowPyrLK(prevGray, gray, // 2 consecutive images
                             trackedPoints[0], // input point position in first image
            trackedPoints[1], // output point postion in the second image
            pointStatus,    // tracking success
            trackingError);      // tracking error
}

void OpticalFlowTracker::removeIncorrectTrackedPoints() {
    int k = 0;
    for (int i = 0; i < trackedPoints[1].size(); i++) {

        if (isOpticalFlowPointCorrect(i)) {
            initialPosition[k] = initialPosition[i];
            trackedPoints[1][k++] = trackedPoints[1][i];
        }
    }

    // eliminate unsuccesful points
    trackedPoints[1].resize(k);
    initialPosition.resize(k);
}


// determine which tracked point should be accepted
// here we keep only moving points
bool OpticalFlowTracker::isOpticalFlowPointCorrect(int i) {

    return pointStatus[i] && // status is false if unable to track point i
            // if point has moved
            (abs(trackedPoints[0][i].x - trackedPoints[1][i].x) +
            (abs(trackedPoints[0][i].y - trackedPoints[1][i].y)) > 2);
}

// handle the currently tracked points
void OpticalFlowTracker::handleTrackedPoints(cv::Mat &frame, cv::Mat &output) {

    // for all tracked points
    for (int i = 0; i < trackedPoints[1].size(); i++) {
        cv::circle(output, trackedPoints[1][i], 3, cv::Scalar(0, 0, 255), -1);
    }
}

void OpticalFlowTracker::swapPoints() {
    std::swap(trackedPoints[1], trackedPoints[0]);
    cv::swap(prevGray, gray);
}

std::vector<cv::Point2f> OpticalFlowTracker::getTrackedPoints() {
    return trackedPoints[0];
}
