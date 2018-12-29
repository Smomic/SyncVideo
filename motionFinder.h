#ifndef MOTIONFINDER_H
#define MOTIONFINDER_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <algorithm>

class MotionFinder {

    std::vector<cv::Point2f> trackedFeatures;
    std::vector<std::vector<cv::Point>> imageContours;
    std::vector<cv::Rect> boundingRectangles;
    const int opticalFlowDistance = 50;

    void findImageContours(cv::Mat &);
    void findBoundingRectangles();
    void drawRectangles(cv::Mat &);
    cv::Mat createMask(cv::Mat &);
    cv::Rect getMovingObjectContour(cv::Mat &);

    static cv::Point2f getCenter(const cv::Rect &rectangle) {
        return 0.5 * (rectangle.br() + rectangle.tl());
    }

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

public:
    MotionFinder();
    void setTrackedFeatures(const std::vector<cv::Point2f> &);
    void process(cv::Mat &, cv::Mat &, cv::Mat &);

};


#endif //MOTIONFINDER_H
