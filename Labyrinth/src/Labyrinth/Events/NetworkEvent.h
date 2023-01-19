#pragma once

#include <Labyrinth/Core/UUID.h>
#include <Labyrinth/Networking/Message.h>

#include "EventTypes.h"

namespace Laby {

    struct NetMessageEvent
    {
        UUID source;
        Message message;

        NetMessageEvent(UUID src, Message&& msg) : source(src), message(msg) {}

        EVENT_DATA_TYPE(NetMessage)
	};
}