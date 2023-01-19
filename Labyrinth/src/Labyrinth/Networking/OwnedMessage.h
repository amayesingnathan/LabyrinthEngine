#pragma once

#include "Connection.h"
#include "Message.h"

namespace Laby {

	struct OwnedMessage
	{
		Ref<Connection> remote = nullptr;
		Message msg;

		friend std::ostream& operator << (std::ostream& os, const OwnedMessage& msg)
		{
			os << msg.msg;
			return os;
		}
	};
}