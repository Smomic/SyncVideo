#ifndef OUTPUTPROCESSOR_H
#define OUTPUTPROCESSOR_H

#include <iomanip>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

class OutputProcessor
{
    cv::VideoWriter writer;

public:
    bool set(const std::string &, cv::Size, double, bool = true);
    void writeNextFrame(cv::Mat &);
    void release();

private:
    int calculateCodec(char [4]);

};

#endif // OUTPUTPROCESSOR_H
