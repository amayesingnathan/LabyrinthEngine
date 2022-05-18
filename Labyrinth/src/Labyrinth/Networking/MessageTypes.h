#pragma once

#include <cstdint>

namespace Labyrinth {

	namespace Net {

		enum class MessageTypes : uint32_t
		{
			ServerAccept,
			ServerDeny,
			ServerPing,
			MessageAll,
			ServerMessage,
		};
	}
}