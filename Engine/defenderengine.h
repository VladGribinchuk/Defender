#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>
#include <vector>

#include "processutil.h"

namespace data_transmission
{
class MessageReceiver;
class MessageSender;
}

class QTimer;

namespace defender_engine
{

class DefenderEngine : public QObject
{
    Q_OBJECT

public:
    explicit DefenderEngine();
    ~DefenderEngine();

    /* Message communication */
    void setPort(int port);
    int getPort() const;
    void sendStartUpMessage();

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

signals:
    void receiveStartUpMessage(const QString&);
    void receiveShutDownMessage(const QString&);
    void receiveIAmOnlineMessage(const QString&);

    /* Processes monitoring */
    void updateRunningProcesses(const ProcessesList&);

    /* System info */
    void systemInfoChanged();

    /* Error reporting */
    void error(const QString&);

private:
    /* Message communication */
    void setUpDataTransmittion();
    void sendShutDownMessage();
    void sendIAmOnlineMessage(const QString&);

private:
    std::unique_ptr<data_transmission::MessageReceiver> mReceiver;
    std::unique_ptr<data_transmission::MessageSender> mSender;

    QTimer* mTimer;
};

}
