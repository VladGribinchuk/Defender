#include "engine.h"
#include "enginesettings.h"
#include "startuputil.h"
#include "systeminfoutil.h"

#include "messagereceiver.h"
#include "messagesender.h"
#include "message.h"
#include "logger.h"

#include <QSettings>
#include <QSysInfo>
#include <QHostInfo>
#include <QTimer>
#include <QCoreApplication>

#include <algorithm>

namespace defender_engine
{

using namespace data_transmission;

Engine::Engine()
{
    int port = getPort();
    mReceiver.reset(new MessageReceiver(port));
    mSender.reset(new MessageSender(port));

    setUpDataTransmittion();

    connect(this, &Engine::error, &Logger::instance(), &Logger::error);

    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, [this](){ emit updateRunningProcesses(getAllRunningProcesses()); });
    mTimer->start(getTimeout());

    Logger::instance().message(QString("Engine is initialized. Listen on port: %1").arg(port));
}

Engine::~Engine()
{
    sendShutDownMessage();
    Logger::instance().message("Engine is deinitialized");
}

void Engine::setPort(int port)
{
    if (mSender.get() != nullptr)
        sendShutDownMessage();

    mReceiver.reset(new MessageReceiver(port));
    mSender.reset(new MessageSender(port));

    setUpDataTransmittion();

    sendStartUpMessage();

    EngineSettings::instance().setValue("port", port);
    Logger::instance().message(QString("Communication port set to %1").arg(port));
}

int Engine::getPort() const
{
    return EngineSettings::instance().value("port", data_transmission::GetDefaultPort()).toInt();
}

void Engine::setTimeout(int ms)
{
    mTimer->stop();
    mTimer->start(ms);

    EngineSettings::instance().setValue("timeout", ms);
    Logger::instance().message(QString("Processes monitoring timeout set to %1 ms").arg(ms));
}

int Engine::getTimeout() const
{
    return EngineSettings::instance().value("timeout", 1000).toInt();
}

ProcessesList Engine::getAllRunningProcesses() const
{
    return ProcessUtil::instance().getAllRunningProcess();
}

void Engine::killProcess(long id) const
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

void Engine::setStartUp(bool isStartUp)
{
    StartUpUtil::setStartUp("DefenderApp", isStartUp);

    EngineSettings::instance().setValue("startup", isStartUp);
    Logger::instance().message(QString("Application startup on system boot is %1").arg(QString(isStartUp?"on":"off")));
}

bool Engine::getStartUp() const
{
    return EngineSettings::instance().value("startup", false).toBool();
}

QString Engine::getSystemInfo() const
{
    return EngineSettings::instance().value("sysinfo", "").toString();
}

void Engine::setUpDataTransmittion()
{
    if (mReceiver.get() == nullptr || mSender.get() == nullptr)
        return;

    connect(mReceiver.get(), &MessageReceiver::recieveMessage, this, [this](const Message& msg, const QString& sender){
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
    });

    connect(mReceiver.get(), &MessageReceiver::error, this, &Engine::error);
    connect(mSender.get(), &MessageSender::error, this, &Engine::error);
}

void Engine::sendStartUpMessage()
{
    mSender->broadcast(Message(Message::MessageType::Activated));
    Logger::instance().message("Send startup message");
}

void Engine::sendShutDownMessage()
{
    mSender->broadcast(Message(Message::MessageType::Deactivated));
    Logger::instance().message("Send shutdown message");
}

void Engine::sendIAmOnlineMessage(const QString& recipient)
{
    mSender->send(Message(Message::MessageType::Greetings), recipient);
    Logger::instance().message(QString("Send greeting to %1").arg(recipient));
}

void Engine::checkSysInfo()
{
    QString sysinfo = SystemInfoUtil::getSystemInfo();
    QString grabbed = EngineSettings::instance().value("sysinfo", "").toString();
    if (!grabbed.isEmpty() && sysinfo != grabbed)
    {
        Logger::instance().warning("System configuration was chaged");
        emit systemInfoChanged();
    }
    EngineSettings::instance().setValue("sysinfo", sysinfo);
}

}
