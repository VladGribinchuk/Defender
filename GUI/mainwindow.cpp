#include "mainwindow.h"
#include "defenderengine.h"
#include "logger.h"
#include "defenderenginesettings.h"
#include "views.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPixmap>
#include <QHBoxLayout>

#include <functional>

namespace defender_gui
{

using namespace defender_engine;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createEngine();
    createUI();

    setupEngine();
    setupUI();

    connect(&Logger::instance(), &Logger::logging, this, [this](const QString& msg){
        statusBar()->showMessage(msg, 4000);
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!mTrayIcon->isVisible())
    {
        QMessageBox::information(this,"Defender Application", "Please note the program will keep running in the background mode."
                                                              "To terminate the program, choose <b>Quit</b> in the context menu "
                                                              "of the system tray entry.");
        hide();
        mTrayIcon->show();
        event->ignore();
    }
}

void MainWindow::createEngine()
{
    mEngine = std::unique_ptr<defender_engine::DefenderEngine>(new DefenderEngine);
}

void MainWindow::createUI()
{
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(new QHBoxLayout);
    ((QHBoxLayout*)centralWidget()->layout())->addWidget(mNodesView = new NodesView, 2);
    ((QHBoxLayout*)centralWidget()->layout())->addWidget(mProcessesView = new ProcessesView, 2);
    ((QHBoxLayout*)centralWidget()->layout())->addWidget(mSettingsView = new SettingsView, 0, Qt::AlignTop);
    mTrayIcon = new QSystemTrayIcon(this);
}

void MainWindow::setupEngine()
{
    connect(mEngine.get(), &DefenderEngine::systemInfoChanged, this, [this](){
        QMessageBox::information(this, "Defender Application", "System info was changes since last startup.");
    });

    connect(mEngine.get(), &DefenderEngine::receiveIAmOnlineMessage, mNodesView, &NodesView::addNode);
    connect(mEngine.get(), &DefenderEngine::receiveStartUpMessage, mNodesView, &NodesView::addNode);
    connect(mEngine.get(), &DefenderEngine::receiveShutDownMessage, mNodesView, &NodesView::removeNode);
    connect(mEngine.get(), &DefenderEngine::updateRunningProcesses, mProcessesView, &ProcessesView::updateProcessesList);

    connect(mProcessesView, &ProcessesView::stopProcess, mEngine.get(), &DefenderEngine::killProcess);
    connect(mSettingsView, &SettingsView::portChanged, this, [this](int port){
        mNodesView->clearNodes();
        mEngine->setPort(port);
    });
    connect(mSettingsView, &SettingsView::timeoutChanged, mEngine.get(), &DefenderEngine::setTimeout);
    connect(mSettingsView, &SettingsView::startUpChanged, mEngine.get(), &DefenderEngine::setStartUp);

    mEngine->checkSysInfo();
    mEngine->sendStartUpMessage();
}

void MainWindow::setupUI()
{
    {   // Main window setup
        setWindowTitle("Defender App");
        setWindowIcon(QPixmap(":/icons/guard.png"));
        resize(800, 500);
    }

    {   // Menu bar setup
        connect(menuBar()->addMenu("File")->addAction("Quit"), &QAction::triggered, qApp, &QApplication::quit);
        connect(menuBar()->addMenu("Info")->addAction("About"), &QAction::triggered, this, [this](){
            QMessageBox::information(this, "Defender Application",
                                     "All rights reserved (c) 2018\n");
        });
    }

    {   // Views setup
        mSettingsView->setData(mEngine->getPort(), mEngine->getTimeout(), mEngine->getStartUp(), mEngine->getSystemInfo());
    }

    {   // Tray setup
        auto trayMenu = new QMenu(this);
        connect(trayMenu->addAction("Restore"), &QAction::triggered, this, [this] (){
            showNormal();
            mTrayIcon->hide();
        });
        connect(trayMenu->addAction("Quit"), &QAction::triggered, qApp, &QApplication::quit);
        mTrayIcon->setContextMenu(trayMenu);
        mTrayIcon->setIcon(QPixmap(":/icons/guard.png"));
    }
}

}
