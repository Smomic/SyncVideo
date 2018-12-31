#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "videoController.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    controller(new VideoController()) {

    ui->setupUi(this);
    ui->stopButton->setEnabled(false);
    ui->gridLayout = qobject_cast<QGridLayout*>(ui->centralWidget->layout());
    ui->label->setAlignment(Qt::AlignCenter);
    setWindowTitle(windowName);
    connect(controller, &VideoController::sendFrame, this, &MainWindow::onFrameProcessed);
    connect(controller, &VideoController::inputEnded, this, &MainWindow::onVideoEnded);
    connect(controller, &VideoController::openingInputError, this, &MainWindow::onOpeningInputError);
    connect(controller, &VideoController::openingOutputError, this, &MainWindow::onOpeningOutputError);
    connect(controller, &VideoController::readingInputError, this, &MainWindow::onReadingInputError);
}

MainWindow::~MainWindow() {
    delete controller;
    delete ui;
}

void MainWindow::on_actionOpen_First_Video_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load First Video"), "/home/michal/SyncVideo/data",
                                                    tr("Videos (") + tr(acceptedVideoFormats) +  tr(")"));
    if (!isInputFileNameCorrect(fileName)
            || (oneVideoSet && !checkInputFrameSize(fileName)))
        return;

    controller->setFirstInputFileName(fileName.toUtf8().constData());
    controller->setFirstInputPosition(1);
    ui->firstTimeSlider->setEnabled(true);
    ui->firstTimeSlider->setMaximum(controller->getNumberOfFrames(false));
    windowTitle() == tr(windowName) ? setWindowTitle(fileName)
                                    : setWindowTitle(fileName + tr("\tvs\t") + windowTitle());
    setStartButtonEnabled();
    setResetActionEnabled(true);
    ui->firstBackward->setEnabled(true);
    ui->firstForward->setEnabled(true);
    setSaveActionEnabled(true);
    setLoadFirstVideoActionEnabled(false);
}

void MainWindow::on_actionOpen_Second_Video_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load Second Video"), "/home/michal/SyncVideo/data",
                                                    tr("Videos (") + tr(acceptedVideoFormats) +  tr(")"));
    if (!isInputFileNameCorrect(fileName) ||
            (oneVideoSet && !checkInputFrameSize(fileName)))
        return;

    controller->setSecondInputFileName(fileName.toUtf8().constData());
    controller->setSecondInputPosition(1);
    ui->secondTimeSlider->setEnabled(true);
    ui->secondTimeSlider->setMaximum(controller->getNumberOfFrames(true));
    windowTitle() == tr(windowName) ? setWindowTitle(fileName)
                                    : setWindowTitle(windowTitle() + tr("\tvs\t") + fileName);
    setStartButtonEnabled();
    setResetActionEnabled(true);
    ui->secondForward->setEnabled(true);
    ui->secondBackward->setEnabled(true);
    setSaveActionEnabled(true);
    setLoadSecondVideoActionEnabled(false);
}

bool MainWindow::isInputFileNameCorrect(QString name) {
    if (name.isEmpty())
        return false;

    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Unable to open file"),
                             name);
        return false;
    }
    return true;
}

bool MainWindow::checkInputFrameSize(QString fileName) {
    if(oneVideoSet
            && !controller->checkInputFrameSize(fileName.toUtf8().constData())
            && !errorOccurred) {
        QMessageBox::warning(this, tr("Incorrect frame size"),
                             tr("Frame size must be the same in both videos"));
        return false;
    }

    if (errorOccurred) {
        errorOccurred = false;
        return false;
    }

    return true;
}

void MainWindow::on_actionSave_Output_Video_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Output Video"), "/home/michal/SyncVideo/data",
                                                    tr("Videos (") + tr(acceptedVideoFormats) +  tr(")"));
    if (fileName.isEmpty())
        return;

    controller->setOutputFileName(fileName.toUtf8().constData());
}

void MainWindow::setSaveActionEnabled(bool value) {
    ui->menuSyncVideo->actions().at(2)->setEnabled(value);
}

void MainWindow::on_actionReset_triggered() {
    ui->label->setPixmap(QPixmap());
    controller->setFirstInputFileName("");
    controller->setSecondInputFileName("");
    controller->setOutputFileName("");
    setWindowTitle(windowName);
    setButtonsEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->firstTimeSlider->setValue(0);
    ui->secondTimeSlider->setValue(0);
    setLoadFirstVideoActionEnabled(true);
    setLoadSecondVideoActionEnabled(true);
    setResetActionEnabled(false);
    setSaveActionEnabled(false);
    oneVideoSet = false;
    image = QImage();
}

void MainWindow::setResetActionEnabled(bool value) {
    ui->menuSyncVideo->actions().at(3)->setEnabled(value);
}


void MainWindow::setLoadFirstVideoActionEnabled(bool value) {
    ui->menuSyncVideo->actions().at(0)->setEnabled(value);

}

void MainWindow::setLoadSecondVideoActionEnabled(bool value) {
    ui->menuSyncVideo->actions().at(1)->setEnabled(value);

}

void MainWindow::on_startButton_pressed() {
    setButtonsEnabled(false);
    synchronizingStarted = true;
    setResetActionEnabled(false);
    setSaveActionEnabled(false);
    controller->run();
}

void MainWindow::on_stopButton_pressed() {
    setButtonsEnabled(true);
    synchronizingStarted = false;
    setResetActionEnabled(true);
    setSaveActionEnabled(true);
    controller->stop();
}

void MainWindow::setButtonsEnabled(bool value) {
    ui->startButton->setEnabled(value);
    ui->stopButton->setEnabled(!value);
    ui->firstTimeSlider->setEnabled(value);
    ui->secondTimeSlider->setEnabled(value);
    ui->firstBackward->setEnabled(value);
    ui->firstForward->setEnabled(value);
    ui->secondForward->setEnabled(value);
    ui->secondBackward->setEnabled(value);
}

void MainWindow::setStartButtonEnabled() {
    if(!oneVideoSet)
        oneVideoSet = true;
    else
        ui->startButton->setEnabled(true);
}

void MainWindow::onFrameProcessed(cv::Mat &frame) {
    if(!frame.empty()) {
        if (frame.channels()== 3)
            viewColorImage(frame);
        else
            viewGreyImage(frame);

        if (synchronizingStarted)
            changeSliderOnProcessed();
    }
    qApp->processEvents();
}

void MainWindow::viewColorImage(cv::Mat &frame) {
    QImage qimg(frame.data, frame.cols, frame.rows, int(frame.step), QImage::Format_RGB888);
    image = qimg.rgbSwapped();
    setImageInLabel();
}

void MainWindow::viewGreyImage(cv::Mat &frame) {
    image = QImage(frame.data, frame.cols,frame.rows,QImage::Format_Indexed8);
    setImageInLabel();
}

void MainWindow::setImageInLabel() {
    if (shouldImageScaled(image.size(), ui->label->size()))
        ui->label->setPixmap(QPixmap::fromImage(image).scaled(ui->label->size(),
                                                              Qt::KeepAspectRatio, Qt::FastTransformation));
    else
        ui->label->setPixmap(QPixmap::fromImage(image));
}

bool MainWindow::shouldImageScaled(QSize imageSize, QSize labelSize) {
    return imageSize.width() > labelSize.width() || imageSize.height() > labelSize.height();
}

void MainWindow::changeSliderOnProcessed() {
    changeTimeSliderPosition(ui->firstTimeSlider, 1);
    changeTimeSliderPosition(ui->secondTimeSlider, 1);
}

void MainWindow::on_firstTimeSlider_sliderMoved(int position) {
    controller->setFirstInputPosition(position);
}

void MainWindow::on_secondTimeSlider_sliderMoved(int position) {
    controller->setSecondInputPosition(position);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    if (!ui->label->pixmap())
        return;

    setImageInLabel();
    QMainWindow::resizeEvent(event);
}

void MainWindow::on_actionQuit_triggered() {
    if (synchronizingStarted) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Closing application"),
                                                                  tr("Do you want stop synchronization?"));
        if(reply == QMessageBox::No)
            return;
        else
            on_stopButton_pressed();
    }
    QCoreApplication::quit();
}

void MainWindow::on_firstBackward_pressed() {
    changeTimeSliderPosition(ui->firstTimeSlider, -changePositionButtonStep);
    controller->changeFirstInputPosition(-changePositionButtonStep);
}

void MainWindow::on_firstForward_pressed() {
    changeTimeSliderPosition(ui->firstTimeSlider, changePositionButtonStep);
    controller->changeFirstInputPosition(changePositionButtonStep);
}

void MainWindow::on_secondBackward_pressed() {
    changeTimeSliderPosition(ui->secondTimeSlider, -changePositionButtonStep);
    controller->changeSecondInputPosition(-changePositionButtonStep);
}

void MainWindow::on_secondForward_pressed() {
    changeTimeSliderPosition(ui->secondTimeSlider, changePositionButtonStep);
    controller->changeSecondInputPosition(changePositionButtonStep);
}

void MainWindow::changeTimeSliderPosition(QSlider *slider, int value) {
    int pos = slider->value() + value;
    if (pos >= 0 && pos <= slider->maximum())
        slider->setValue(pos);
}

void MainWindow::onVideoEnded() {
    setButtonsEnabled(true);
    synchronizingStarted = false;
    setSaveActionEnabled(true);
    setResetActionEnabled(true);
}

void MainWindow::onOpeningInputError(const std::string &name) {
    QMessageBox::warning(this, tr("Opening input error"),
                         tr("Cannot open ") + tr(name.c_str()));
    errorOccurred = true;
}

void MainWindow::onOpeningOutputError(const std::string &name) {
    QMessageBox::warning(this, tr("Opening output error"),
                         tr("Cannot open ") + tr(name.c_str()));
}

void MainWindow::onReadingInputError(const std::string &name) {
    QMessageBox::warning(this, tr("Reading input error"),
                         tr("Cannot open ") + tr(name.c_str()));
    errorOccurred = true;
}
