#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>

namespace data_transmission
{

class Message
{
public:
    enum class MessageType : char
    {
        NonType,
        Activated,
        Deactivated,
        Greetings
    };

    Message() :
        mType(MessageType::NonType)
    {}

    explicit Message(MessageType type) :
        mType(type)
    {}

    void append(const QByteArray& data) { mBody.append(data); }
    void append(const char* data, size_t len) { std::copy(data, data + len, std::back_inserter(mBody));}

    MessageType type() const { return mType; }
    const QByteArray& body() const { return mBody; }
    bool valid() const { return mType != MessageType::NonType; }

    QByteArray& body() { return mBody; }

    QByteArray toQByteArray() const;
    static Message fromQByteArray(const QByteArray& data);

private:
    MessageType mType;
    QByteArray mBody;
};

}

#endif // MESSAGE_H
