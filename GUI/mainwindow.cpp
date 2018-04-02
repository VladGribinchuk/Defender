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
    else
    {
        mEngine->stopAnyRunningTask();
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
    connect(mEngine.get(), &DefenderEngine::receiveStateSwitchingMessage, [this](const QString& nodeId, EngineStates state){
        QString msg = QString("starting working on: ") + toString(state);
        mNodesView->modifyNode(nodeId, msg);
    });
    connect(mEngine.get(), &DefenderEngine::receiveStateCompleteMessage, [this](const QString& nodeId, EngineStates state){
        QString msg = QString("completed working on: ") + toString(state);
        mNodesView->modifyNode(nodeId, msg);
    });
    connect(mEngine.get(), &DefenderEngine::receiveCurrentStateMessage, [this](const QString& nodeId, EngineStates state){
        QString msg = QString("currently working on: ") + toString(state);
        mNodesView->modifyNode(nodeId, msg);
    });
    connect(mEngine.get(), &DefenderEngine::currentStateChanged, [this](EngineStates state){
        mNodesView->setCurrentStatus(toString(state));
    });
    connect(mEngine.get(), &DefenderEngine::updateCurrentStateInfo, mNodesView, &NodesView::updateCurrentStatusInfo);
    connect(mEngine.get(), &DefenderEngine::infectedFileWasFound, [this](QString file){
        QMessageBox::critical(this, "Defender Application", "Infected file was found during scanning: " + file);
    });
    connect(mEngine.get(), &DefenderEngine::infectedFileWasFoundFrom, [this](QString from, QString file){
        QMessageBox::critical(this, "Defender Application", QString("Infected file %1 was found during scanning by %2: ").arg(file).arg(from));
    });
    connect(mEngine.get(), &DefenderEngine::infectedProcessWasFound, [this](QString process){
        QMessageBox::critical(this, "Defender Application", "Infected process was found during scanning: " + process);
    });
    connect(mEngine.get(), &DefenderEngine::infectedProcessWasFoundFrom, [this](QString from, QString process){
        QMessageBox::critical(this, "Defender Application", QString("Infected process %1 was found during scanning by %2: ").arg(process).arg(from));
    });

    connect(mNodesView, &NodesView::stopCurrentTask, mEngine.get(), &DefenderEngine::stopAnyRunningTask);

    connect(mEngine.get(), &DefenderEngine::updateRunningProcesses, mProcessesView, &ProcessesView::updateProcessesList);

    connect(mProcessesView, &ProcessesView::stopProcess, mEngine.get(), &DefenderEngine::killProcess);
    connect(mSettingsView, &SettingsView::portChanged, this, [this](int port){
        mNodesView->clearNodes();
        mEngine->setPort(port);
    });
    connect(mSettingsView, &SettingsView::timeoutChanged, mEngine.get(), &DefenderEngine::setTimeout);
    connect(mSettingsView, &SettingsView::startUpChanged, mEngine.get(), &DefenderEngine::setStartUp);
    connect(mSettingsView, &SettingsView::scanningDirChanged, mEngine.get(), &DefenderEngine::setScanningDir);
    connect(mEngine.get(), &DefenderEngine::scanningDirChanged, mSettingsView, &SettingsView::setScanningDir);
    connect(mSettingsView, &SettingsView::periodChanged, mEngine.get(), &DefenderEngine::setWorkingPeriod);

    mEngine->checkSysInfo();
    mEngine->sendStartUpMessage();
    mEngine->sendPollOutMessage();
}

void MainWindow::setupUI()
{
    {   // Main window setup
        setWindowTitle("Defender App");
        setWindowIcon(QPixmap(":/icons/guard.png"));
        resize(800, 500);
    }

    {   // Menu bar setup
        connect(menuBar()->addMenu("File")->addAction("Quit"), &QAction::triggered, [this](){
            mEngine->stopAnyRunningTask();
            qApp->quit();
        });
        connect(menuBar()->addMenu("Info")->addAction("About"), &QAction::triggered, this, [this](){
            QMessageBox::information(this, "Defender Application",
                                     "All rights reserved (c) 2018\n");
        });
    }

    {   // Views setup
        mSettingsView->setData(mEngine->getPort(), mEngine->getTimeout(), mEngine->getStartUp(), mEngine->getSystemInfo(), mEngine->getWorkingPeriod());
        mNodesView->setCurrentIP(mEngine->getLocalIpAdress());
        mNodesView->setCurrentStatus("Starting up work...");
    }

    {   // Tray setup
        auto trayMenu = new QMenu(this);
        connect(trayMenu->addAction("Restore"), &QAction::triggered, this, [this] (){
            showNormal();
            mTrayIcon->hide();
        });
        connect(trayMenu->addAction("Quit"), &QAction::triggered, [this](){
            mEngine->stopAnyRunningTask();
            qApp->quit();
        });

        mTrayIcon->setContextMenu(trayMenu);
        mTrayIcon->setIcon(QPixmap(":/icons/guard.png"));
    }
}

}
