#include "Lpch.h"
#include "MessageBus.h"

#include "Labyrinth/IO/YAML.h"

namespace Labyrinth {

    void MessageBus::Init()
    {
    }

    void MessageBus::Shutdown()
    {
    }

    void MessageBus::Send(Message& message)
    {
        if (!message.getFilter().valid())
            return;

        sMessages.emplace_back(std::move(message));
    }

    void MessageBus::Notify()
    {
        for (const auto& message : sMessages)
        {
            if (sReceivers.count(message.getFilter()) != 0)
            {
                for (MessageCallback callback : sReceivers[message.getFilter()])
                    callback(message);
            }
        }
    }

    void MessageBus::AddMessageType(const std::string& label)
    {
        if (sMessageTypes.count(label) != 0)
        {
            LAB_CORE_WARN("Message Type already exists!");
            return;
        }

        sMessageTypes[label] = UUID();
    }

    MessageType MessageBus::GetMessageType(const std::string& label)
    {
        if (sMessageTypes.count(label) == 0)
            return 0;

        return sMessageTypes.at(label);
    }

}