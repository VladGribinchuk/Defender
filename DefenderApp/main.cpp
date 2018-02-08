#include "GUI/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    defender_gui::MainWindow w;
    w.show();

    return a.exec();
}
