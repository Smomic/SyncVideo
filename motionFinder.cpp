#include "motionFinder.h"

MotionFinder::MotionFinder() : trackedFeatures(0), imageContours(0), boundingRectangles(0) {}

void MotionFinder::setTrackedFeatures(const std::vector<cv::Point2f> &trackedFeatures) {
    MotionFinder::trackedFeatures = trackedFeatures;
}

void MotionFinder::process(cv::Mat &frame, cv::Mat &output, cv::Mat &mask) {
    findImageContours(frame);
    findBoundingRectangles();
    createMask(output).copyTo(mask);
    //drawRectangles(output);

    boundingRectangles.clear();
}

void MotionFinder::findImageContours(cv::Mat &source) {
    cv::Mat greyMatrix;
    cvtColor(source, greyMatrix, cv::COLOR_BGR2GRAY);
    blur(source, greyMatrix, cv::Size(3, 3));
    cv::Mat openingMatrix;
    cv::morphologyEx(greyMatrix, openingMatrix, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9)));
    cv::Mat cannyMatrix;
    std::vector<cv::Vec4i> hierarchy;

    double med = computeMedian(openingMatrix, source.type()*1024);
    int lower = int(fmax(0, 0.67*med));
    int higher = int(fmin(255, 1.33*med));

    Canny(openingMatrix, cannyMatrix, lower, higher, 3);

    cv::Mat d, closingMatrix;
    cv::threshold(cannyMatrix, d, 200, 255, cv::THRESH_BINARY);
    for(int i = 0; i < 3; ++i) {
        cv::morphologyEx(d, closingMatrix, cv::MORPH_DILATE,
                         cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9)));
        d = closingMatrix;
    }

    for(int i = 0; i < 3; ++i) {
        cv::morphologyEx(d, closingMatrix, cv::MORPH_ERODE,
                         cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9)));
        d = closingMatrix;
    }

    /// Find contours
    findContours(d, imageContours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    //   cv::namedWindow("Canny", cv::WINDOW_NORMAL);
    // cv::resizeWindow("Canny", 1280, 720);
    //  imshow("Canny", d);

    // Draw contours
    //        cv::RNG rng(12345);
    //        cv::Mat drawing = cv::Mat::zeros(closingMatrix.size(), CV_8UC3);

    //        for (int i = 0; i < imageContours.size(); i++) {
    //            cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    //            cv::drawContours(drawing, imageContours, i, color, 2, 8, hierarchy, 0, cv::Point());
    //            //cv::drawContours(drawing, hull, i, color, 2, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
    //        }

    //        namedWindow("Contours", cv::WINDOW_NORMAL);
    //        cv::resizeWindow("Contours", 1280, 720);
    //        imshow("Contours", drawing);
}

void MotionFinder::findBoundingRectangles() {
    for (const auto &imageContour : imageContours)
        boundingRectangles.push_back(boundingRect(cv::Mat(imageContour)));
}

void MotionFinder::drawRectangles(cv::Mat &output) {
    for (auto const &value : boundingRectangles)
        rectangle(output, value, cv::Scalar(0, 255, 0), 2, 8, 0);
}

cv::Mat MotionFinder::createMask(cv::Mat &output) {
    cv::Mat mask = cv::Mat::zeros(output.size(), CV_8U);
    cv::Mat roi = cv::Mat(mask, cv::Rect(getMovingObjectContour(output)));
    //cv::Mat(mask, cv::Rect(100, 970, 1250, 70));
    cv::bitwise_not(mask, mask);
    return mask;
}

cv::Rect MotionFinder::getMovingObjectContour(cv::Mat &output) {
    std::map<unsigned, unsigned> myMap;

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
    if (!myMap.empty()) {
        unsigned value = myMap.rbegin()->first;
        cv::Rect rect(boundingRect(cv::Mat(imageContours[value])));
        // rectangle(output, rect, cv::Scalar(0, 255, 0), 2, 8, 0);
        return rect;
    }

    return cv::Rect();
}
