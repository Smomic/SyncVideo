#include "resultScreen.h"

ResultScreen::ResultScreen(QWidget *parent) : QLabel(parent) {}

void ResultScreen::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        mousePressed = true;
        maskRectangle.setTopLeft(event->pos());
        maskRectangle.setBottomRight(event->pos());
    }
}

void ResultScreen::mouseMoveEvent(QMouseEvent *event) {
        if(event->buttons() & Qt::LeftButton) {

        }
    if (event->type() == QEvent::MouseMove && (event->buttons() & Qt::LeftButton))
        maskRectangle.setBottomRight(event->pos());

    update();
}

void ResultScreen::paintEvent(QPaintEvent *event) {
    painter.begin(this);

    if (mousePressed) {
        if (pixMap.width() > width() || pixMap.height() > height())
            pixMap = pixMap.scaled(size(), Qt::KeepAspectRatio, Qt::FastTransformation);

        painter.drawPixmap(0, 0, pixMap);
        painter.drawRect(maskRectangle);
        //setImageInLabel();
        drawStarted = true;
            painter.end();

        //update();
        return;
        } else if (drawStarted) {
            QPainter tempPainter(&pixMap);
            tempPainter.drawRect(maskRectangle);
            painter.drawPixmap(0, 0, pixMap);
        }
    painter.end();
}

void ResultScreen::mouseReleaseEvent(QMouseEvent *event){

    //When mouse is released update for the one last time
    mousePressed = false;
    update();
}

void ResultScreen::setPixmap(const QPixmap &pMap) {

    pixMap = pMap;

}

const QPixmap ResultScreen::ResultScreen::getPixmap() {
    return pixMap;
}

