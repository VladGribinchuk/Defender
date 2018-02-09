#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "defender_gui_global.h"

#include <QMainWindow>
#include <memory>

class QSystemTrayIcon;
class QCloseEvent;

namespace defender_engine
{
class Engine;
}

namespace defender_gui
{

class NodesView;
class ProcessesView;
class SettingsView;

class DEFENDER_GUI_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void createEngine();
    void createUI();
    void setupEngine();
    void setupUI();

private:
    /* Core functionality */
    std::unique_ptr<defender_engine::Engine> mEngine;

    /* UI elements */
    QSystemTrayIcon* mTrayIcon;
    NodesView* mNodesView;
    ProcessesView* mProcessesView;
    SettingsView* mSettingsView;
};

}

#endif // MAINWINDOW_H
