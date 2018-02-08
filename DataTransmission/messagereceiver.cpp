#include "datatransmission.h"
#include "messagereceiver.h"
#include "message.h"

#include <QString>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>

namespace data_transmission
{

MessageReceiver::MessageReceiver(int port) :
    MessageTransmission(port)
{
    mUdpSocket->bind(mPort, QUdpSocket::ShareAddress);
    connect(mUdpSocket, &QUdpSocket::readyRead, this, [this](){
        while (mUdpSocket->hasPendingDatagrams())
        {
            QHostAddress sender;
            quint16 port;
            QByteArray datagram(static_cast<int>(mUdpSocket->pendingDatagramSize()), '\0');
            auto readBytes = mUdpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &port);

            if (readBytes == -1)
                emit error(mUdpSocket->errorString());

            if (!isLocalIp(sender.toString()))
                emit recieveMessage(Message::fromQByteArray(datagram), sender.toString());
        }
    });
}

bool MessageReceiver::isLocalIp(const QString& ip)
{
    for (const auto& address : QNetworkInterface::allAddresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost) && address.toString() == ip)
            return true;
    }
    return false;
}

}
