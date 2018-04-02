#include "defenderengine.h"
#include "defenderenginesettings.h"
#include "startuputil.h"
#include "systeminfoutil.h"
#include "virusscanner.h"

#include "messagereceiver.h"
#include "messagesender.h"
#include "logger.h"

#include <QSettings>
#include <QSysInfo>
#include <QHostInfo>
#include <QTimer>
#include <QTime>
#include <QCoreApplication>
#include <QStandardPaths>

#include <QNetworkInterface>
#include <QHostAddress>

#include <algorithm>
#include <thread>
#include <QThread>

namespace defender_engine
{

using namespace data_transmission;

DefenderEngine::DefenderEngine()
    : mCurrentState(EngineStates::Waiting)
{
    int port = getPort();

    init(port);

    connect(this, &DefenderEngine::error, &Logger::instance(), &Logger::error);

    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, [this](){ emit updateRunningProcesses(getAllRunningProcesses()); });
    mTimer->start(getTimeout());

    Logger::instance().message(QString("Engine is initialized. Listen on port: %1").arg(port));

    mScheduler = new QTimer(this);
    connect(mScheduler, &QTimer::timeout, this, &DefenderEngine::scheduleTask);
    mScheduler->start(2000);

    mProgressTimer = new QTimer(this);
    connect(mProgressTimer, &QTimer::timeout, [this](){
        if (getCurrentState() != EngineStates::Waiting)
            emit updateCurrentStateInfo();
    });
    mProgressTimer->start(1000);
}

DefenderEngine::~DefenderEngine()
{
    sendShutDownMessage();
    Logger::instance().message("Engine is deinitialized");
}

void DefenderEngine::init(int port)
{
    mReceiver.reset(new MessageReceiver(port));
    mSender.reset(new MessageSender(port));

    setUpDataTransmittion();
}

void DefenderEngine::scheduleTask()
{
    if (getCurrentState() != EngineStates::Waiting) // engine is busy at the moment
    {
        return;
    }

    qsrand((uint)QTime::currentTime().msec());
    int msec = 1000 * getWorkingPeriod() /** 60*/;
    mScheduler->start(msec);
    Logger::instance().state("Scheduling next task. Working period = " + QString::number(msec / 1000 / 60) + " minutes.");

    int next = (qrand() % 3);
    EngineStates nextState;
    if (next == 0)
    {
        nextState = EngineStates::CheckingFiles;
        QStringList list = QStandardPaths::standardLocations(static_cast<QStandardPaths::StandardLocation>(qrand() % 18));
        if (!list.empty())
        {
            //setScanningDir(list.front());
            //emit scanningDirChanged(list.front());
            setScanningDir("C://");
            emit scanningDirChanged("C://");
        }
    }
    else if (next == 1)
    {
        nextState = EngineStates::CheckingProcesses;
    }
    else if (next == 2)
    {
        nextState = EngineStates::OptimizingLogs;
    }

    mWorkerThread = new WorkerThread(this);
\
    connect(mWorkerThread, &WorkerThread::finished, [this](){
        mWorkerThread->deleteLater();
    });

    connect(mWorkerThread, &WorkerThread::updateProgress, this, &DefenderEngine::updateCurrentStateInfo);

    mWorkerThread->setParams(this, nextState);
    mWorkerThread->start();
}

void DefenderEngine::stopAnyRunningTask()
{
    if (getCurrentState() == EngineStates::Waiting)
        return;

    if (mWorkerThread != nullptr && mWorkerThread->isRunning())
        mWorkerThread->requestInterruption();
}

void DefenderEngine::setPort(int port)
{
    if (mSender.get() != nullptr)
        sendShutDownMessage();

    init(port);

    sendStartUpMessage();
    sendPollOutMessage();

    DefenderEngineSettings::instance().setValue("port", port);
    Logger::instance().message(QString("Communication port set to %1").arg(port));
}

int DefenderEngine::getPort() const
{
    return DefenderEngineSettings::instance().value("port", data_transmission::GetDefaultPort()).toInt();
}

void DefenderEngine::setTimeout(int ms)
{
    mTimer->stop();
    mTimer->start(ms);

    DefenderEngineSettings::instance().setValue("timeout", ms);
    Logger::instance().message(QString("Processes monitoring timeout set to %1 ms").arg(ms));
}

int DefenderEngine::getTimeout() const
{
    return DefenderEngineSettings::instance().value("timeout", 1000).toInt();
}

ProcessesList DefenderEngine::getAllRunningProcesses() const
{
    return ProcessUtil::instance().getAllRunningProcess();
}

void DefenderEngine::killProcess(long id) const
{
    QString errorStr;
    if (!ProcessUtil::instance().killProcessById(id, errorStr))
    {
        Logger::instance().error(QString("Process terminating error: %1").arg(errorStr));
    }
    else
    {
        Logger::instance().message(QString("Process [%1] was terminated").arg(id));
    }
}

void DefenderEngine::setStartUp(bool isStartUp)
{
    StartUpUtil::setStartUp("DefenderApp", isStartUp);

    DefenderEngineSettings::instance().setValue("startup", isStartUp);
    Logger::instance().message(QString("Application startup on system boot is %1").arg(QString(isStartUp?"on":"off")));
}

bool DefenderEngine::getStartUp() const
{
    return DefenderEngineSettings::instance().value("startup", false).toBool();
}

QString DefenderEngine::getSystemInfo() const
{
    return DefenderEngineSettings::instance().value("sysinfo", "").toString();
}

void DefenderEngine::sendPollOutMessage()
{
    mSender->broadcast(Message(Message::MessageType::PollOut));
    Logger::instance().message("Send poll out message to get info about current states of all nodes");
}

void DefenderEngine::sendCurrentStateMessage(const QString& recipient)
{
    Message msg(Message::MessageType::CurrentState);
    QByteArray data;
    data.append(static_cast<char>(getCurrentState()));
    msg.append(data);
    mSender->send(msg, recipient);
    Logger::instance().message(QString("Send current state to %1").arg(recipient));
}

FilesInfo DefenderEngine::checkFilesInDir(const QString& dir)
{
    setCurrentState(EngineStates::CheckingFiles);
    sendStateSwitchingInfo(EngineStates::CheckingFiles);
    Logger::instance().state(QString("Start working on task: %1 in %2").arg(toString(EngineStates::CheckingFiles)).arg(dir));

    auto res = FileUtil::checkDirectory(dir);
    if (QThread::currentThread()->isInterruptionRequested())
    {
        setCurrentState(EngineStates::Waiting);
        sendStateSwitchingInfo(EngineStates::Waiting);
        return res;
    }

    auto msgData = CheckingFilesMessageData(dir, res.totalDirCount, res.totalFilesCount, res.exeFiles.size());
    sendStateCompleteInfo(&msgData);
    Logger::instance().state(QString("Complete work on task: %1 in %2, results: total dirs count = %3, total files count = %4, total exe files = %5")
                             .arg(toString(EngineStates::CheckingFiles))
                             .arg(dir)
                             .arg(res.totalDirCount)
                             .arg(res.totalFilesCount)
                             .arg(res.exeFiles.size()));

    // decide whether switch to next level
    if (QTime::currentTime().msec() % 2)
    {
        scanFiles(res);
    }

    setCurrentState(EngineStates::Waiting);
    sendStateSwitchingInfo(EngineStates::Waiting);
    return res;
}

ProcessesList DefenderEngine::checkRunningProcesses()
{
    setCurrentState(EngineStates::CheckingProcesses);
    sendStateSwitchingInfo(EngineStates::CheckingProcesses);
    Logger::instance().state(QString("Start working on task: %1").arg(toString(EngineStates::CheckingProcesses)));

    auto res = ProcessUtil::instance().getAllRunningProcess();
    if (QThread::currentThread()->isInterruptionRequested())
    {
        setCurrentState(EngineStates::Waiting);
        sendStateSwitchingInfo(EngineStates::Waiting);
        return res;
    }

    auto msgData = CheckingProcessesMessageData(res.size());
    sendStateCompleteInfo(&msgData);
    Logger::instance().state(QString("Complete work on task: %1, results: running processes = %2")
                             .arg(toString(EngineStates::CheckingProcesses))
                             .arg(res.size()));

    // decide whether switch to next level
    if (QTime::currentTime().msec() % 2)
    {
        scanProcesses(res);
    }

    setCurrentState(EngineStates::Waiting);
    sendStateSwitchingInfo(EngineStates::Waiting);
    return res;
}

void DefenderEngine::scanFiles(const FilesInfo& files)
{
    setCurrentState(EngineStates::ScanningFiles);
    sendStateSwitchingInfo(EngineStates::ScanningFiles);
    Logger::instance().state(QString("Start working on task: %1").arg(toString(EngineStates::ScanningFiles)));

    long infected = 0;
    for (const auto& file : files.exeFiles)
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            setCurrentState(EngineStates::Waiting);
            sendStateSwitchingInfo(EngineStates::Waiting);
            return;
        }

        if (VirusScanner::scan(file.path))
        {
            sendInfectedFileWasFound(file.path);
            Logger::instance().critical(QString("Infected file was found: %1").arg(file.path));
            emit infectedFileWasFound(file.path);
            infected++;
        }
    }
    auto msgData = ScanningFilesMessageData(files.exeFiles.size(), infected);
    sendStateCompleteInfo(&msgData);
    Logger::instance().state(QString("Complete work on task: %1, results: scanning files = %2, infected files = %3")
                             .arg(toString(EngineStates::ScanningFiles))
                             .arg(files.exeFiles.size())
                             .arg(infected));
}

void DefenderEngine::scanProcesses(const ProcessesList& processes)
{
    setCurrentState(EngineStates::ScanningProcesses);
    sendStateSwitchingInfo(EngineStates::ScanningProcesses);
    Logger::instance().state(QString("Start working on task: %1").arg(toString(EngineStates::ScanningProcesses)));

    long infected = 0;
    for (const auto& process : processes)
    {
        if (VirusScanner::scan(ProcessUtil::instance().exePath(process.pid)))
        {
            if (QThread::currentThread()->isInterruptionRequested())
            {
                setCurrentState(EngineStates::Waiting);
                sendStateSwitchingInfo(EngineStates::Waiting);
                return;
            }
            sendInfectedProcessWasFound(process.name);
            Logger::instance().critical(QString("Infected process was found: %1").arg(process.name));
            emit infectedProcessWasFound(process.name);
            infected++;
        }
    }
    auto msgData = ScanningProcessesMessageData(processes.size(), infected);
    sendStateCompleteInfo(&msgData);
    Logger::instance().state(QString("Complete work on task: %1, results: scanned processes = %2, infected processes = %3")
                             .arg(toString(EngineStates::ScanningProcesses))
                             .arg(processes.size())
                             .arg(infected));
}

void DefenderEngine::optimizeLog()
{
    setCurrentState(EngineStates::OptimizingLogs);
    sendStateSwitchingInfo(EngineStates::OptimizingLogs);
    Logger::instance().state(QString("Start working on task: %1").arg(toString(EngineStates::OptimizingLogs)));

    auto res = Logger::instance().optimize();

    auto msgData = OptimizingLogsMessageData(res.deletedEntries, res.sizeBefore, res.sizeAfter);
    sendStateCompleteInfo(&msgData);
    Logger::instance().state(QString("Complete work on task: %1, results: deleted entries = %2, file size before = %3, file size after = %4")
                             .arg(toString(EngineStates::OptimizingLogs))
                             .arg(res.deletedEntries)
                             .arg(res.sizeBefore)
                             .arg(res.sizeAfter));
    setCurrentState(EngineStates::Waiting);
    sendStateSwitchingInfo(EngineStates::Waiting);
}

int DefenderEngine::getWorkingPeriod() const
{
    return DefenderEngineSettings::instance().value("workingPeriod", 1).toInt();
}

void DefenderEngine::setWorkingPeriod(int period)
{
    DefenderEngineSettings::instance().setValue("workingPeriod", period);
}

void DefenderEngine::setUpDataTransmittion()
{
    if (mReceiver.get() == nullptr || mSender.get() == nullptr)
        return;

    connect(mReceiver.get(), &MessageReceiver::recieveMessage, this, [this](const Message& msg, const QString& sender){
        try
        {
            if (msg.type() == Message::MessageType::Activated)
            {
                Logger::instance().message(QString("Receive startup message from %1").arg(sender));
                emit receiveStartUpMessage(sender);

                sendIAmOnlineMessage(sender);
            }
            else if (msg.type() == Message::MessageType::Deactivated)
            {
                Logger::instance().message(QString("Receive shutdown message from %1").arg(sender));
                emit receiveShutDownMessage(sender);
            }
            else if (msg.type() == Message::MessageType::Greetings)
            {
                Logger::instance().message(QString("Receive greetings from %1").arg(sender));
                emit receiveIAmOnlineMessage(sender);
            }
            else if (msg.type() == Message::MessageType::StatesSwitching)
            {
                Logger::instance().state(QString("%1 started working on: %2").arg(sender).arg(toString(EngineStates(msg.body()[0]))));
                emit receiveStateSwitchingMessage(sender, EngineStates(static_cast<char>(msg.body()[0])));
            }
            else if (msg.type() == Message::MessageType::StateComplete)
            {
                auto msgData = msg.getStatesMsg();
                EngineStates state = msgData->state();

                if (state == EngineStates::CheckingFiles)
                {
                    CheckingFilesMessageData* ptr = (CheckingFilesMessageData*)msgData.get();
                    QString path = ptr->dir();
                    long totalDirsCount = ptr->totalDirsCount();
                    long totalFilesCount = ptr->totalFilesCount();
                    long totalExeFilesCount = ptr->exeFiles();
                    Logger::instance().state(QString("%1 completed working on task: %2 in %3, results: total dirs count = %4, total files count = %5, total exe files = %6")
                                             .arg(sender)
                                             .arg(toString(state))
                                             .arg(path)
                                             .arg(totalDirsCount)
                                             .arg(totalFilesCount)
                                             .arg(totalExeFilesCount));
                }
                else if (state == EngineStates::CheckingProcesses)
                {
                    CheckingProcessesMessageData* ptr = (CheckingProcessesMessageData*)msgData.get();
                    long runningProcesses = ptr->runningProcesses();
                    Logger::instance().state(QString("%1 completed working on task: %2, results: running processes = %3")
                                             .arg(sender)
                                             .arg(toString(EngineStates::CheckingProcesses))
                                             .arg(runningProcesses));
                }
                else if (state == EngineStates::OptimizingLogs)
                {
                    OptimizingLogsMessageData* ptr = (OptimizingLogsMessageData*)msgData.get();
                    long deletedEntries = ptr->deletedEntries();
                    long sizeBefore = ptr->sizeBefore();
                    long sizeAfter = ptr->sizeAfter();
                    Logger::instance().state(QString("%1 completed working on task: %2, results: deleted entries = %3, file size before = %4, file size after = %5")
                                             .arg(sender)
                                             .arg(toString(EngineStates::OptimizingLogs))
                                             .arg(deletedEntries)
                                             .arg(sizeBefore)
                                             .arg(sizeAfter));
                }
                else if (state == EngineStates::ScanningFiles)
                {
                    ScanningFilesMessageData* ptr = (ScanningFilesMessageData*)msgData.get();
                    long scannedFiles = ptr->scannedFiles();
                    long infectedFiles = ptr->infectedFiles();
                    Logger::instance().state(QString("%1 completed working on task: %2, results: scanned files = %3, infected files = %4")
                                             .arg(sender)
                                             .arg(toString(EngineStates::ScanningFiles))
                                             .arg(scannedFiles)
                                             .arg(infectedFiles));
                }
                else if (state == EngineStates::ScanningProcesses)
                {
                    ScanningProcessesMessageData* ptr = (ScanningProcessesMessageData*)msgData.get();
                    long scannedProcesses = ptr->scannedProcesses();
                    long infectedProcesses = ptr->infectedProcesses();
                    Logger::instance().state(QString("%1 completed working on task: %2, results: scanned processes = %3, infected processes = %4")
                                             .arg(sender)
                                             .arg(toString(EngineStates::ScanningFiles))
                                             .arg(scannedProcesses)
                                             .arg(infectedProcesses));
                }
                emit receiveStateCompleteMessage(sender, state);
            }
            else if (msg.type() == Message::MessageType::PollOut)
            {
                Logger::instance().message(QString("Receive poll out message from %1").arg(sender));
                sendCurrentStateMessage(sender);
            }
            else if (msg.type() == Message::MessageType::CurrentState)
            {
                Logger::instance().message(QString("Receive current state message from %1").arg(sender));
                emit receiveCurrentStateMessage(sender, EngineStates(msg.body()[0]));
            }
            else if (msg.type() == Message::MessageType::InfectedFileWasFound)
            {
                Logger::instance().message(QString("Receive message from %1 about founding infected file").arg(sender));
                QString file = QString::fromStdString(msg.body().toStdString());
                emit infectedFileWasFoundFrom(sender, file);
            }
            else if (msg.type() == Message::MessageType::InfectedProcessWasFound)
            {
                Logger::instance().message(QString("Receive message from %1 about founding infected process").arg(sender));
                QString process = QString::fromStdString(msg.body().toStdString());
                emit infectedProcessWasFoundFrom(sender, process);
            }
        }
        catch (...)
        {
            emit error(QString("Unexpected error during receiving message from ") + sender);
        }
    });

    connect(mReceiver.get(), &MessageTransmission::error, this, &DefenderEngine::error);
    connect(mSender.get(), &MessageTransmission::error, this, &DefenderEngine::error);
}

void DefenderEngine::sendStartUpMessage()
{
    mSender->broadcast(Message(Message::MessageType::Activated));
    Logger::instance().message("Send startup message");
}

QString DefenderEngine::getLocalIpAdress() const
{
    QString ret;
    int i = 0;
    for (const auto& address : QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            ret += "interface" + QString::number(i++) + ": " + address.toString() + "\n";
    }
    return ret;
}

void DefenderEngine::sendShutDownMessage()
{
    mSender->broadcast(Message(Message::MessageType::Deactivated));
    Logger::instance().message("Send shutdown message");
}

void DefenderEngine::sendIAmOnlineMessage(const QString& recipient)
{
    mSender->send(Message(Message::MessageType::Greetings), recipient);
    Logger::instance().message(QString("Send greeting to %1").arg(recipient));
}

void DefenderEngine::sendStateSwitchingInfo(EngineStates state)
{
    Message msg(Message::MessageType::StatesSwitching);
    QByteArray data;
    data.append(static_cast<char>(state));
    msg.append(data);
    mSender->broadcast(msg);
    Logger::instance().message(QString("Send message about starting woking on: %1").arg(toString(state)));
}

void DefenderEngine::sendStateCompleteInfo(StatesMessageData* msgData)
{
    Message msg(Message::MessageType::StateComplete);
    msg.setFrom(msgData);
    mSender->broadcast(msg);
    Logger::instance().message(QString("Send message about completing working on: %1").arg(toString(msgData->state())));
}

void DefenderEngine::sendInfectedFileWasFound(const QString& file)
{
    Message msg(Message::MessageType::InfectedFileWasFound);
    QByteArray ba = QByteArray::fromStdString(file.toStdString());
    msg.append(ba);
    mSender->broadcast(msg);
    Logger::instance().message(QString("Send message about founding infected file: %1").arg(file));
}

void DefenderEngine::sendInfectedProcessWasFound(const QString& process)
{
    Message msg(Message::MessageType::InfectedProcessWasFound);
    QByteArray ba = QByteArray::fromStdString(process.toStdString());
    msg.append(ba);
    mSender->broadcast(msg);
    Logger::instance().message(QString("Send message about founding infected process: %1").arg(process));
}

EngineStates DefenderEngine::getCurrentState()
{
    std::lock_guard<std::mutex> lock(mStateMutex);
    return mCurrentState;
}

void DefenderEngine::setCurrentState(EngineStates state)
{
    std::lock_guard<std::mutex> lock(mStateMutex);
    mCurrentState = state;
    emit currentStateChanged(mCurrentState);
}

void DefenderEngine::checkSysInfo()
{
    QString sysinfo = SystemInfoUtil::getSystemInfo();
    QString grabbed = DefenderEngineSettings::instance().value("sysinfo", "").toString();
    if (!grabbed.isEmpty() && sysinfo != grabbed)
    {
        Logger::instance().warning("System configuration was chaged");
        emit systemInfoChanged();
    }
    DefenderEngineSettings::instance().setValue("sysinfo", sysinfo);
}

void DefenderEngine::setScanningDir(const QString& dir)
{
    mScanningDir = dir;
}

QString DefenderEngine::getScanningDir() const
{
    return mScanningDir;
}

void WorkerThread::run()
{
    switch (state)
    {
    case EngineStates::Waiting:
        break;
    case EngineStates::CheckingFiles:
        engine->checkFilesInDir(engine->getScanningDir());
        break;
    case EngineStates::CheckingProcesses:
        engine->checkRunningProcesses();
        break;
    case EngineStates::OptimizingLogs:
        engine->optimizeLog();
        break;
    default:
        break;
    }
    emit resultReady();
}

}
