#pragma once

#include "Message.h"

#include <Labyrinth/Core/System/Cast.h>

namespace Labyrinth {

    class MessageBus
    {
    public:
        using MessageCallback = std::function<void(const Message&)>;

    public:
        static void AddReceiver(MessageFilter filter, MessageCallback callback)
        {
            LAB_CORE_ASSERT(filter.valid(), "Must specify filter parameters!");
            sReceivers[filter].emplace_back(callback);
        }

        static void ClearReceiver(MessageFilter filter) { sReceivers.erase(filter); }
        static void Clear() { sReceivers.clear(); }

        static void AddMessageType(const std::string& label);
        static void RemoveMessageType(const std::string& label) { sMessageTypes.erase(label); }
        static MessageType GetMessageType(const std::string& label);

    private:
        static void Init();
        static void Shutdown();

        static void Send(Message& message);
        static void Notify();

    private:
        inline static std::unordered_map<MessageFilter, std::vector<MessageCallback>> sReceivers;
        inline static std::unordered_map<std::string, MessageType> sMessageTypes;
        inline static std::vector<Message> sMessages;

        friend class BusNode;
        friend class MessageBusPanel;
    };

#define BUS_NODE_ID(expression) NodeID getNodeID() const override { return expression; }

    class BusNode
    {
    public:
        BusNode() {}

        virtual NodeID getNodeID() const = 0;
        void send(Message& message) const { MessageBus::Send(message); }

        Message newMessage(const std::string& type, NodeID other) const { return Message({ getNodeID(), other, MessageBus::GetMessageType(type) }); }
        Message newMessage(const std::string& type, NodeID other, Buffer& data) const { return Message({ getNodeID(), other, MessageBus::GetMessageType(type) }, data); }
    };
}