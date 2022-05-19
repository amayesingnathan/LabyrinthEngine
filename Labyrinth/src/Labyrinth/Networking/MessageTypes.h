#pragma once

#include <cstdint>

namespace Labyrinth {

	namespace Net {

		using MessageType = uint32_t;

		namespace MessageTypes {

			enum : MessageType {
				ServerAccept,
				ServerDeny,
				ServerPing,
				MessageAll,
				ServerMessage,
			};
		}
	}
}