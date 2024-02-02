#pragma once

#include <Labyrinth/Containers/StreamlineTypes.h>

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

		u32 Size() const
		{
			return StaticCast<u32>(body.Size());
		}

		friend std::ostream& operator << (std::ostream& os, const Message& msg)
		{
			os << "ID:" << i32(msg.header.id) << " Size:" << msg.header.size;
			return os;
		}

		template<slc::IsStandard DataType>
		friend Message& operator<< (Message& msg, const DataType& data)
		{
			usize i = msg.body.Size();
			msg.body.Set(data, i);
			msg.header.size = msg.Size();

			return msg;
		}

		template<slc::IsStandard DataType>
		friend Message& operator>> (Message& msg, DataType& data)
		{
			msg.body.Pop(data);
			msg.header.size = msg.Size();

			return msg;
		}

	};
}