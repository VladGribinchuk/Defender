#include "datatransmission.h"

#include <QUdpSocket>

namespace data_transmission
{

MessageTransmission::MessageTransmission() :
    MessageTransmission(GetDefaultPort())
{}

MessageTransmission::MessageTransmission(int port) :
    mPort(port),
    mUdpSocket(new QUdpSocket(this))
{}

}
