/*
 * Title: Spatial synchronization of video sequences
 * Author: Michał Smoła
 */

#ifndef CONTOURFINDER_H
#define CONTOURFINDER_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <algorithm>
#include <map>

class ContourFinder {

    std::vector<std::vector<cv::Point>> imageContours;
    unsigned acceptedContourCount;
    int morphRadius;
    const double ratio = 0.33;

public:
    ContourFinder();
    void process(cv::Mat &);
    std::vector<std::vector<cv::Point>> getImageContours();

private:
    static double calculateMedian(cv::Mat inputMatrix, int n) {
        cv::Mat cdf;
        calculateHistogram(inputMatrix, n).copyTo(cdf);
        calculateCDF(cdf, inputMatrix, n);

        double medianVal = 0;
        for (int i = 0; i < n; i++) {
            if (cdf.at<float>(i) >= 0.5){
                medianVal = i;
                break;
            }
        }
        return medianVal;
    }

    static cv::Mat calculateHistogram(cv::Mat inputMatrix, int n) {
        float range[] = {0, float(n)};
        const float *hRange = {range};
        bool uniform = true;
        bool accumulate = false;
        cv::Mat histogram;
        int channels[] = {0};
        calcHist(&inputMatrix, 1, channels, cv::Mat(), histogram, 1, &n, &hRange, uniform, accumulate);
        return histogram;
    }

    static void calculateCDF(cv::Mat &cdf, cv::Mat inputMatrix, int n) {
        for (int i = 1; i < n; i++){
            cdf.at<float>(i) += cdf.at<float>(i - 1);
        }
        cdf /= inputMatrix.total();
    }

    void calculateCanny(cv::Mat &, cv::Mat &, int);
    void findImageContours(cv::Mat &);
    void morphologyProcess(cv::Mat &, int, int);
};

#endif // CONTOURFINDER_H
