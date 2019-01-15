#ifndef CONTOURFINDER_H
#define CONTOURFINDER_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opticalFlowTracker.h"
#include <algorithm>
#include <map>

class ContourFinder {

    std::vector<std::vector<cv::Point>> imageContours;
    const double ratio = 0.33;
    const unsigned acceptedContourCount = 50;

public:
    ContourFinder() {}
    void process(cv::Mat &);
    std::vector<std::vector<cv::Point>> getImageContours();
private:
    static double computeMedian(cv::Mat inputMatrix, int n) {
        // COMPUTE HISTOGRAM OF SINGLE CHANNEL MATRIX
        float range[] = {0, float(n)};
        const float* histRange = {range};
        bool uniform = true;
        bool accumulate = false;
        cv::Mat histogram;
        int channels[] = {0};
        calcHist(&inputMatrix, 1, channels, cv::Mat(), histogram, 1, &n, &histRange, uniform, accumulate);

        // COMPUTE CUMULATIVE DISTRIBUTION FUNCTION (CDF)
        cv::Mat cdf;
        histogram.copyTo(cdf);
        for (int i = 1; i < n; i++){
            cdf.at<float>(i) += cdf.at<float>(i - 1);
        }
        cdf /= inputMatrix.total();

        // COMPUTE MEDIAN
        double medianVal = 0;
        for (int i = 0; i < n; i++) {
            if (cdf.at<float>(i) >= 0.5){
                medianVal = i;
                break;
            }
        }
        return medianVal;
    }

    void calculateCanny(cv::Mat &, cv::Mat &, int);
    void findImageContours(cv::Mat &);
    void morphologyProcess(cv::Mat &, int, int);
};

#endif // CONTOURFINDER_H
