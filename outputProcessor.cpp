#include "outputProcessor.h"

bool OutputProcessor::set(const std::string &fileName, cv::Size frameSize, double frameRate, bool isColor) {
    char c[4];
    int codec = calculateCodec(c);

    return writer.open(fileName, codec, frameRate, frameSize, isColor);
}

void OutputProcessor::release() {
    writer.release();
}

void OutputProcessor::writeNextFrame(cv::Mat &frame) {
    writer.write(frame);
}

int OutputProcessor::calculateCodec(char codec[4]) {
    union {
        int value;
        char code[4];
    } returned;

    cv::VideoCapture capture;
    returned.value = static_cast<int>(capture.get(cv::CAP_PROP_FOURCC));

    codec[0] = returned.code[0];
    codec[1] = returned.code[1];
    codec[2] = returned.code[2];
    codec[3] = returned.code[3];

    return returned.value;
}
