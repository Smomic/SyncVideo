#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPixmap>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QSlider>
#include <QPainter>

#include <opencv2/opencv.hpp>
#include "videoController.h"
#include "mainScene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    Ui::MainWindow *ui;
    VideoController *controller;
    MainScene *scene;
    QImage image;
    QGraphicsPixmapItem *pixMapItem;

    const char *acceptedFormats = "*.avi *.mp4 *.mpg";
    const char *windowName = "SyncVideo";
    const int changePositionButtonStep = 1;
    bool oneVideoSet = false;
    bool synchronizingStarted = false;
    bool errorOccurred = false;

public:
    explicit MainWindow(QWidget * = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_First_Video_triggered();
    void on_actionOpen_Second_Video_triggered();
    void on_actionSave_Output_Video_triggered();
    void on_actionQuit_triggered();
    void on_actionReset_triggered();
    void on_startButton_pressed();
    void on_stopButton_pressed();
    void on_firstTimeSlider_sliderMoved(int);
    void on_secondTimeSlider_sliderMoved(int);
    void on_firstBackward_pressed();
    void on_firstForward_pressed();
    void on_secondBackward_pressed();
    void on_secondForward_pressed();

    void onFrameProcessed(cv::Mat &);
    void onVideoEnded();
    void onOpeningInputError(const std::string &);
    void onOpeningOutputError(const std::string &);
    void onReadingInputError(const std::string &);

private:
    bool isInputFileNameCorrect(QString);
    void viewColorImage(cv::Mat &);
    void viewGreyImage(cv::Mat &);
    void setImageInLabel();

    void setButtonsEnabled(bool);
    void changeTimeSliderPosition(QSlider *, int);
    void setResetActionEnabled(bool);
    void setLoadFirstVideoActionEnabled(bool);
    void setLoadSecondVideoActionEnabled(bool);
    void setStartButtonEnabled();
    bool checkInputFrameSize(QString);
    void setSaveActionEnabled(bool);
    void changeSliderOnProcessed();
    bool shouldImageScaled(QSize, QSize);
protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *);

};

#endif // MAINWINDOW_H
