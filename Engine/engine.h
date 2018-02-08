#pragma once

#include <QObject>
#include <memory>

class QString;

namespace data_transmission
{
class MessageReceiver;
class MessageSender;
}

namespace defender_engine
{

class Engine : public QObject
{
    Q_OBJECT

public:
    explicit Engine(int portForCommunictaion);
    ~Engine();

    /* Message communication */
    void setPort(int port);
    void sendStartUpMessage();
    void sendShutDownMessage();

signals:
    void receiveStartUpMessage(const QString&);
    void receiveShutDownMessage(const QString&);

    /* Error reporting */
    void error(const QString&);

private:
    std::unique_ptr<data_transmission::MessageReceiver> mReceiver;
    std::unique_ptr<data_transmission::MessageSender> mSender;
};

}
