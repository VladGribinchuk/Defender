#include "GUI/mainwindow.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(0, "Systray", "No any system tray was detected on this system!");
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    defender_gui::MainWindow w;
    w.show();

    return a.exec();
}
