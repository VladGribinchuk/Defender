#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include "datatransmission.h"

class QString;
class QUdpSocket;

namespace data_transmission
{

class Message;

class MessageSender : public MessageTransmission
{
    Q_OBJECT

public:
    MessageSender() = default;
    explicit MessageSender(int port);

public slots:
    void broadcast(const Message& msg);
    void send(const Message& msg, const QString&);
};

}

#endif // MESSAGESENDER_H
