#include "message.h"

namespace data_transmission
{

QByteArray Message::toQByteArray() const
{
    QByteArray result;
    result.push_back(static_cast<char>(mType));
    result.append(mBody);
    return result;
}

Message Message::fromQByteArray(const QByteArray& data)
{
    Message message;
    if (!data.isEmpty())
    {
        message.mType = static_cast<MessageType>(data[0]);
        std::copy(data.begin() + 1, data.end(), std::back_inserter(message.mBody));
    }
    return message;
}

}
