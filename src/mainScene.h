/*
 * Title: Spatial synchronization of video sequences
 * Author: Michał Smoła
 */

#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QRect>

class MainScene : public QGraphicsScene {
    Q_OBJECT

    QGraphicsRectItem *mask;
    std::vector<QGraphicsRectItem*> maskRectangles;
    QSize pixmapSize;
    bool drawStarted = false;
    bool drawEnabled = false;

    void rightButtonMousePressEvent(QGraphicsSceneMouseEvent *);
    void leftButtonMousePressEvent(QGraphicsSceneMouseEvent *);
    void checkMousePosition(QGraphicsSceneMouseEvent *);
    bool checkCorners(QGraphicsSceneMouseEvent *);
    void checkEdges(QGraphicsSceneMouseEvent *);

public:
    MainScene(QObject* = 0) {}
    void reset();
    void setDrawEnabled(bool);
    std::vector<QGraphicsRectItem*> getMaskRectangles();
    void setPixmapSize(QSize size);
    void hideMaskRectangles();
    void showMaskRectangles();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

};

#endif // MAINSCENE_H
