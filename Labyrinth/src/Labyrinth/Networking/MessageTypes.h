#pragma once

#include <cstdint>

namespace Labyrinth {

	namespace Net {

		using MessageType = u32;

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