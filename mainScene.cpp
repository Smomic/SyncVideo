#include "mainScene.h"

void MainScene::reset() {
    if (!maskRectangles.empty()) {
        for(auto &m : maskRectangles)
            removeItem(m);

        maskRectangles.clear();
    }
    update();
}

void MainScene::setDrawEnabled(bool value) {
    drawEnabled = value;
}

void MainScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (!drawEnabled)
        return;

    if (event->button() == Qt::RightButton)
        rightButtonMousePressEvent(event);

    else if (event->button() == Qt::LeftButton)
        leftButtonMousePressEvent(event);
}

void MainScene::rightButtonMousePressEvent(QGraphicsSceneMouseEvent *event) {
    for(size_t i = 0; i < maskRectangles.size(); ++i){
        QGraphicsRectItem *m = maskRectangles.at(i);
        if (m->boundingRect().contains(event->scenePos())) {
            removeItem(m);
            delete m;
            maskRectangles.erase(maskRectangles.begin() + i);
            return;
        }
    }
}

void MainScene::leftButtonMousePressEvent(QGraphicsSceneMouseEvent *event) {
    drawStarted = true;
    const QPointF p = event->scenePos();
    if (p.x() > pixmapSize.width() || p.y() > pixmapSize.height() || p.x() < 0 || p.y() < 0)
        return;
    mask = new QGraphicsRectItem(QRectF(event->scenePos(), event->scenePos()));
    mask->setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
    addItem(mask);
    maskRectangles.push_back(mask);
}

void MainScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (drawStarted && mask != 0) {
        checkMousePosition(event);
        QRectF rect(mask->rect().topLeft(), event->scenePos());
        mask->setRect(rect);
    }
    else
        QGraphicsScene::mouseMoveEvent(event);
}

void MainScene::checkMousePosition(QGraphicsSceneMouseEvent *event) {
    const QPointF p = event->scenePos();
    if (p.x() >= pixmapSize.width() && p.y() >= pixmapSize.height()) {
        event->setScenePos(QPointF(pixmapSize.width(), pixmapSize.height()));
    }
    else if (p.x() >= pixmapSize.width())
        event->setScenePos(QPointF(pixmapSize.width(), p.y()));
    else if (p.x() < 0)
        event->setScenePos(QPointF(0, p.y()));
    if (p.y() >= pixmapSize.height())
        event->setScenePos(QPointF(p.x(), pixmapSize.height()));
    else if (p.y() < 0)
        event->setScenePos(QPointF(p.x(), 0));

}

void MainScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    drawStarted = false;
    mask = 0;
    QGraphicsScene::mouseReleaseEvent(event);
}

std::vector<QGraphicsRectItem*> MainScene::getMaskRectangles() {
    return maskRectangles;
}

void MainScene::setPixmapSize(QSize size) {
    pixmapSize = size;
}

void MainScene::hideMaskRectangles() {
    for(auto &m : maskRectangles)
        m->hide();
}

void MainScene::showMaskRectangles() {
    for(auto &m : maskRectangles)
        m->show();
}
