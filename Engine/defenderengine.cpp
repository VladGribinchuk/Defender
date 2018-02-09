#include "defenderengine.h"
#include "defenderenginesettings.h"
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

DefenderEngine::DefenderEngine()
{
    int port = getPort();
    mReceiver.reset(new MessageReceiver(port));
    mSender.reset(new MessageSender(port));

    setUpDataTransmittion();

    connect(this, &DefenderEngine::error, &Logger::instance(), &Logger::error);

    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, [this](){ emit updateRunningProcesses(getAllRunningProcesses()); });
    mTimer->start(getTimeout());

    Logger::instance().message(QString("Engine is initialized. Listen on port: %1").arg(port));
}

DefenderEngine::~DefenderEngine()
{
    sendShutDownMessage();
    Logger::instance().message("Engine is deinitialized");
}

void DefenderEngine::setPort(int port)
{
    if (mSender.get() != nullptr)
        sendShutDownMessage();

    mReceiver.reset(new MessageReceiver(port));
    mSender.reset(new MessageSender(port));

    setUpDataTransmittion();

    sendStartUpMessage();

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

void DefenderEngine::setUpDataTransmittion()
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

    connect(mReceiver.get(), &MessageTransmission::error, this, &DefenderEngine::error);
    connect(mSender.get(), &MessageTransmission::error, this, &DefenderEngine::error);
}

void DefenderEngine::sendStartUpMessage()
{
    mSender->broadcast(Message(Message::MessageType::Activated));
    Logger::instance().message("Send startup message");
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

}
