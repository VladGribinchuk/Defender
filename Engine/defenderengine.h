#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QThread>
#include <memory>
#include <vector>
#include <mutex>

#include "processutil.h"
#include "fileutil.h"
#include "message.h"

namespace data_transmission
{
class MessageReceiver;
class MessageSender;
}

class QTimer;

namespace defender_engine
{

using namespace data_transmission;

class DefenderEngine;

class WorkerThread : public QThread
{
    Q_OBJECT

    void run() override;

public:
    WorkerThread(QObject* parent = 0) : QThread(parent) {}

    void setParams(DefenderEngine* engine, EngineStates state)
    {
        this->state = state;
        this->engine = engine;
    }

signals:
    void resultReady();
    void updateProgress();

private:
    EngineStates state;
    DefenderEngine* engine;
};

class DefenderEngine : public QObject
{
    Q_OBJECT

public:
    explicit DefenderEngine();
    ~DefenderEngine();

    void init(int port);
    void scheduleTask();
    void stopAnyRunningTask();

    /* Message communication */
    void setPort(int port);
    int getPort() const;
    void sendStartUpMessage();
    QString getLocalIpAdress() const;

    /* Processes monitoring */
    void setTimeout(int ms);
    int getTimeout() const;
    ProcessesList getAllRunningProcesses() const;
    void killProcess(long) const;

    /* Engine behaviour */
    void setStartUp(bool);
    bool getStartUp() const;

    /* System info */
    void checkSysInfo();
    QString getSystemInfo() const;

    /* State changing */
    void setScanningDir(const QString& dir);
    QString getScanningDir() const;
    void sendPollOutMessage();
    void sendCurrentStateMessage(const QString&);
    FilesInfo checkFilesInDir(const QString&);
    ProcessesList checkRunningProcesses();
    void scanFiles(const FilesInfo &);
    void scanProcesses(const ProcessesList&);
    void optimizeLog();
    int getWorkingPeriod() const;
    void setWorkingPeriod(int period);

signals:
    void receiveStartUpMessage(const QString& from);
    void receiveShutDownMessage(const QString& from);
    void receiveIAmOnlineMessage(const QString& from);
    void receiveStateSwitchingMessage(const QString& from, EngineStates);
    void receiveStateCompleteMessage(const QString& from, EngineStates);
    void receiveCurrentStateMessage(const QString& from, EngineStates);
    void receiveInfectedFileWasFound(const QString& from, const QString& file);

    /* Processes monitoring */
    void updateRunningProcesses(const ProcessesList&);

    /* System info */
    void systemInfoChanged();

    void currentStateChanged(EngineStates state);
    void updateCurrentStateInfo();
    void infectedFileWasFound(QString);
    void infectedFileWasFoundFrom(QString from, QString file);
    void infectedProcessWasFound(QString);
    void infectedProcessWasFoundFrom(QString from, QString process);
    void scanningDirChanged(QString dir);

    /* Error reporting */
    void error(const QString&);

private:
    /* Message communication */
    void setUpDataTransmittion();
    void sendShutDownMessage();
    void sendIAmOnlineMessage(const QString&);
    void sendStateSwitchingInfo(EngineStates state);
    void sendStateCompleteInfo(StatesMessageData*msg);
    void sendInfectedFileWasFound(const QString&);
    void sendInfectedProcessWasFound(const QString&);

    EngineStates getCurrentState();
    void setCurrentState(EngineStates state);

private:
    std::unique_ptr<data_transmission::MessageReceiver> mReceiver;
    std::unique_ptr<data_transmission::MessageSender> mSender;

    QTimer* mTimer;
    QTimer* mScheduler;
    QTimer* mProgressTimer;

    std::mutex mStateMutex;
    EngineStates mCurrentState;

    WorkerThread* mWorkerThread = nullptr;

    QString mScanningDir;
};

}
