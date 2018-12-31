#include "featureTracker.h"

FeatureTracker::FeatureTracker() : max_count(100000), qlevel(0.1), minDist(5.), finder(new MotionFinder()) {}

void FeatureTracker::process(cv::Mat &frame, cv::Mat &output, cv::Mat &mask) {
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    frame.copyTo(output);

    // 1. if new feature points must be added
    if (addNewPoints()) {
        // detect feature points
        detectFeaturePoints();
        // add the detected features to the currently tracked features
        points[0].insert(points[0].end(), features.begin(), features.end());
        initial.insert(initial.end(), features.begin(), features.end());
    }

    // for first image of the sequence
    if (gray_prev.empty())
        gray.copyTo(gray_prev);

    // 2. track features
    cv::calcOpticalFlowPyrLK(gray_prev, gray, // 2 consecutive images
                             points[0], // input point position in first image
            points[1], // output point postion in the second image
            status,    // tracking success
            err);      // tracking error

    // 3. loop over the tracked points to reject the undesirables
    int k = 0;
    for (int i = 0; i < points[1].size(); i++) {

        // do we keep this point?
        if (acceptTrackedPoint(i)) {

            // keep this point in vector
            initial[k] = initial[i];
            points[1][k++] = points[1][i];
        }
    }

    // eliminate unsuccesful points
    points[1].resize(k);
    initial.resize(k);

    finder->setTrackedFeatures(points[1]);
    finder->process(frame, output, mask);

    // 4. handle the accepted tracked points
    //handleTrackedPoints(frame, output);

    // 5. current points and image become previous ones
    std::swap(points[1], points[0]);
    cv::swap(gray_prev, gray);
}

// feature point detection
void FeatureTracker::detectFeaturePoints() {

    // detect the features
    cv::goodFeaturesToTrack(gray, // the image
                            features,   // the output detected features
                            max_count,  // the maximum number of features
                            qlevel,     // quality level
                            minDist);   // min distance between two features
}

// determine if new points should be added
bool FeatureTracker::addNewPoints() {

    // if too few points
    return points[0].size() <= gray.size().area() / 10000;
}

// determine which tracked point should be accepted
// here we keep only moving points
bool FeatureTracker::acceptTrackedPoint(int i) {

    return status[i] && // status is false if unable to track point i
            // if point has moved
            (abs(points[0][i].x - points[1][i].x) +
            (abs(points[0][i].y - points[1][i].y)) > 2);
}

// handle the currently tracked points
void FeatureTracker::handleTrackedPoints(cv::Mat &frame, cv::Mat &output) {

    // for all tracked points
    for (int i = 0; i < points[1].size(); i++) {
        cv::circle(output, points[1][i], 3, cv::Scalar(0, 0, 255), -1);
    }
}
