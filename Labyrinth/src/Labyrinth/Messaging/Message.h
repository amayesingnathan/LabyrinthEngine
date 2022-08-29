#pragma once

#include <Labyrinth/Core/Buffer.h>
#include <Labyrinth/Core/UUID.h>

#include <unordered_map>

namespace Labyrinth {

    using MessageType = UUID;
    using NodeID = UUID;

    class BusNode;

    struct MessageFilter
    {
        NodeID sender = 0;
        NodeID target = 0;
        MessageType type = 0;

        bool operator==(const MessageFilter& other) const
        {
            bool senderMatch = (sender == 0 || other.sender == 0) ? true : sender == other.sender;
            bool targetMatch = (target == 0 || other.target == 0) ? true : target == other.target;
            bool typeMatch = (type == 0 || other.type == 0) ? true : type == other.type;

            return senderMatch && targetMatch && typeMatch;
        }

        bool valid() const { return sender != 0 && target != 0 && type != 0; }
    };

    class Message
    {
    public:
        Message(const MessageFilter& filter)
            : mFilter(filter) {}
        Message(const MessageFilter& filter, Buffer& data)
            : mFilter(filter), mData(std::move(data)) {}

        const MessageFilter& getFilter() const { return mFilter; }

    private:
        MessageFilter mFilter;
        Buffer mData;
        UUID mID; // For debug
    };
    
}

namespace std {
    template<typename T> struct hash;

    template<>
    struct hash<Labyrinth::MessageFilter>
    {
        size_t operator()(const Labyrinth::MessageFilter& msgHeader) const
        {
            return hash<u64>()(msgHeader.sender) ^ hash<u64>()(msgHeader.target) ^ hash<u64>()(msgHeader.type);
        }
    };
}