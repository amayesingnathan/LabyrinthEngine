#pragma once

#include "NetCommon.h"
#include "MessageTypes.h"

namespace Labyrinth {

	namespace Net {

		struct MessageHeader
		{
			MessageType id{};
			uint32_t size = 0;
		};

		struct Message
		{
			MessageHeader header{};
			std::vector<uint8_t> body;

			uint32_t size() const
			{
				return Cast<uint32_t>(body.size());
			}

			friend std::ostream& operator << (std::ostream& os, const Message& msg)
			{
				os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
				return os;
			}

			template<typename DataType>
			friend Message& operator<< (Message& msg, const DataType& data)
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				size_t i = msg.body.size();

				msg.body.resize(msg.body.size() + sizeof(DataType));

				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				msg.header.size = msg.size();

				return msg;
			}

			template<typename DataType>
			friend Message& operator>> (Message& msg, DataType& data)
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");

				size_t i = msg.body.size() - sizeof(DataType);

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