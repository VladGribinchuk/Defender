#ifndef MESSAGETRANSMISSION_H
#define MESSAGETRANSMISSION_H

#include <QObject>

class QString;
class QUdpSocket;

namespace data_transmission
{

inline int GetDefaultPort() { return 45454; }

class MessageTransmission : public QObject
{
    Q_OBJECT

public:
    MessageTransmission();
    explicit MessageTransmission(int port);

    virtual ~MessageTransmission() {}

signals:
    void error(const QString&);

protected:
    int mPort;
    QUdpSocket* mUdpSocket;
};

}

#endif // MESSAGETRANSMISSION_H
