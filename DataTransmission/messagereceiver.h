#ifndef MESSAGERECEIVER_H
#define MESSAGERECEIVER_H

#include "datatransmission.h"

class QString;
class QUdpSocket;

namespace data_transmission
{

class Message;

class MessageReceiver : public MessageTransmission
{
    Q_OBJECT

public:
    MessageReceiver() = default;
    explicit MessageReceiver(int port);

signals:
    void recieveMessage(const Message&, const QString&);

private:
    bool isLocalIp(const QString&);
};

}

#endif // MESSAGERECEIVER_H
