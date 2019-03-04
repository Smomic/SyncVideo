/*
 * Title: Spatial synchronization of video sequences
 * Author: Michał Smoła
 */

#include "contourFinder.h"

ContourFinder::ContourFinder() : acceptedContourCount(70), morphRadius(9) {}

void ContourFinder::process(cv::Mat &source) {
    cv::Mat greyMatrix;
    cvtColor(source, greyMatrix, cv::COLOR_BGR2GRAY);
    blur(source, greyMatrix, cv::Size(3, 3));
    cv::Mat openingMatrix;
    cv::morphologyEx(greyMatrix, openingMatrix, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(morphRadius, morphRadius)));
    cv::Mat cannyMatrix;
    std::vector<cv::Vec4i> hierarchy;

    calculateCanny(openingMatrix, cannyMatrix, source.type() << 10);

    cv::Mat contoursMatrix;
    cv::threshold(cannyMatrix, contoursMatrix, 200, 255, cv::THRESH_BINARY);
    findImageContours(contoursMatrix);
}

void ContourFinder::calculateCanny(cv::Mat &source, cv::Mat &output, int size) {
    double med = calculateMedian(source, size);
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
                         cv::getStructuringElement(cv::MORPH_RECT, cv::Size(morphRadius, morphRadius)));
        source = morfology.clone();
    }
}

std::vector<std::vector<cv::Point>> ContourFinder::getImageContours() {
    return imageContours;
}
