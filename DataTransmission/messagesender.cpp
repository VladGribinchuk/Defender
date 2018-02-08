#include "datatransmission.h"
#include "messagesender.h"
#include "message.h"

#include <QString>
#include <QUdpSocket>
#include <QHostAddress>

namespace data_transmission
{

MessageSender::MessageSender(int port) :
    MessageTransmission(port)
{}

void MessageSender::broadcast(const Message& msg)
{
    auto bytesSent = mUdpSocket->writeDatagram(msg.toQByteArray(), QHostAddress::Broadcast, mPort);
    if (bytesSent == -1)
        emit error(mUdpSocket->errorString());
}

void MessageSender::send(const Message& msg, const QString& address)
{
    auto bytesSent = mUdpSocket->writeDatagram(msg.toQByteArray(), QHostAddress(address), mPort);
    if (bytesSent == -1)
        emit error(mUdpSocket->errorString());
}

}
