#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

namespace defender_engine
{
class Engine;
}

namespace defender_gui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    /* Core functionality */
    std::unique_ptr<defender_engine::Engine> mEngine;
};

}

#endif // MAINWINDOW_H
