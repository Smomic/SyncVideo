/*
 * Title: Spatial synchronization of video sequences
 * Author: Michał Smoła
 */

#include "mainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
