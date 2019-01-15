#include "contourFinder.h"

void ContourFinder::process(cv::Mat &source) {
    cv::Mat greyMatrix;
    cvtColor(source, greyMatrix, cv::COLOR_BGR2GRAY);
    blur(source, greyMatrix, cv::Size(3, 3));
    cv::Mat openingMatrix;
    cv::morphologyEx(greyMatrix, openingMatrix, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(9, 9)));
    cv::Mat cannyMatrix;
    std::vector<cv::Vec4i> hierarchy;

    calculateCanny(openingMatrix, cannyMatrix, source.type() << 10);

    cv::Mat d, closingMatrix;
    cv::threshold(cannyMatrix, d, 200, 255, cv::THRESH_BINARY);
    findImageContours(d);

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

void ContourFinder::calculateCanny(cv::Mat &source, cv::Mat &output, int size) {
    double med = computeMedian(source, size);
    int lower = int(fmax(0, (1-ratio)*med));
    int higher = int(fmin(255, (1+ratio)*med));

    Canny(source, output, lower, higher, 3);
}

void ContourFinder::findImageContours(cv::Mat &mat) {
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat contours = mat.clone();
    int morfoplogyCounter = 0;
    findContours(contours, imageContours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    while(imageContours.size() > acceptedContourCount) {
        imageContours.clear();
        hierarchy.clear();
        contours = mat.clone();
        morfoplogyCounter++;
        morphologyProcess(contours, cv::MORPH_DILATE, morfoplogyCounter);
        morphologyProcess(contours, cv::MORPH_ERODE, morfoplogyCounter);
        findContours(contours, imageContours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    }
}

void ContourFinder::morphologyProcess(cv::Mat &source, int type, int counter) {
    cv::Mat morfology;
    for (int i = 0; i < counter; ++i) {
        cv::morphologyEx(source, morfology, type,
                         cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9)));
        source = morfology.clone();
    }
}

std::vector<std::vector<cv::Point>> ContourFinder::getImageContours() {
    return imageContours;
}
