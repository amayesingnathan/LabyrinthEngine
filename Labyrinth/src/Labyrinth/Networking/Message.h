#pragma once

#include "NetCommon.h"
#include "MessageTypes.h"

namespace Labyrinth {

	namespace Net {

		struct MessageHeader
		{
			MessageType id{};
			u32 size = 0;
		};

		struct Message
		{
			MessageHeader header{};
			std::vector<u8> body;

			u32 size() const
			{
				return Cast<u32>(body.size());
			}

			friend std::ostream& operator << (std::ostream& os, const Message& msg)
			{
				os << "ID:" << i32(msg.header.id) << " Size:" << msg.header.size;
				return os;
			}

			template<typename DataType>
			friend Message& operator<< (Message& msg, const DataType& data)
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				usize i = msg.body.size();

				msg.body.resize(msg.body.size() + sizeof(DataType));

				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				msg.header.size = msg.size();

				return msg;
			}

			template<typename DataType>
			friend Message& operator>> (Message& msg, DataType& data)
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");

				usize i = msg.body.size() - sizeof(DataType);

				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

				msg.body.resize(i);

				msg.header.size = msg.size();

				return msg;
			}

		};

		class Connection;

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
}