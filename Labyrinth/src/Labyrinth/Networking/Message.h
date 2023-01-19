#pragma once

#include <Labyrinth/Containers/Buffer.h>

#include "NetCommon.h"

namespace Laby {

	using MessageType = u32;

	struct MessageHeader
	{
		MessageType id{};
		u32 size = 0;
	};

	struct Message
	{
		MessageHeader header{};
		Buffer body;

		u32 size() const
		{
			return StaticCast<u32>(body.size());
		}

		friend std::ostream& operator << (std::ostream& os, const Message& msg)
		{
			os << "ID:" << i32(msg.header.id) << " Size:" << msg.header.size;
			return os;
		}

		template<IsStandard DataType>
		friend Message& operator<< (Message& msg, const DataType& data)
		{
			usize i = msg.body.size();
			msg.body.set(data, i);
			msg.header.size = msg.size();

			return msg;
		}

		template<IsStandard DataType>
		friend Message& operator>> (Message& msg, DataType& data)
		{
			msg.body.pop(data);
			msg.header.size = msg.size();

			return msg;
		}

	};
}