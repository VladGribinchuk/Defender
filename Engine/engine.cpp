#include "engine.h"

#include "DataTransmission/messagereceiver.h"
#include "DataTransmission/messagesender.h"
#include "DataTransmission/message.h"

namespace defender_engine
{

using namespace data_transmission;

Engine::Engine(int portForCommunictaion)
{
    setPort(portForCommunictaion);
    connect(mReceiver.get(), &MessageReceiver::recieveMessage, this, [this](const Message& msg, const QString& sender){
        if (msg.type() == Message::MessageType::Activated)
        {
            emit receiveStartUpMessage(sender);
        }
        else if (msg.type() == Message::MessageType::Deactivated)
        {
            emit receiveShutDownMessage(sender);
        }
    });

    connect(mReceiver.get(), &MessageReceiver::error, this, &Engine::error);
    connect(mSender.get(), &MessageSender::error, this, &Engine::error);
}

Engine::~Engine()
{
}

void Engine::setPort(int port)
{
    mReceiver.reset(new MessageReceiver(port));
    mSender.reset(new MessageSender(port));
}

void Engine::sendStartUpMessage()
{
    mSender->broadcast(Message(Message::MessageType::Activated));
}

void Engine::sendShutDownMessage()
{
    mSender->broadcast(Message(Message::MessageType::Deactivated));
}

}
