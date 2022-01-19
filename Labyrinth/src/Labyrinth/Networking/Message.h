#pragma 

#include "Labyrinth/Core/System/Platforms.h"

#ifdef LAB_PLATFORM_WINDOWS
#define _WIN32_WINNT 0x0A00
#endif

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

namespace Labyrinth {

	namespace Net
	{

		template<typename T>
		struct MessageHeader
		{
			T id{};
			uint32_t size = 0;
		};

		template<typename T>
		struct Message
		{
			MessageHeader<T> header{};
			std::vector<uint8_t> body;

			size_t size() const
			{
				return sizeof(MessageHeader<T>) + body.size();
			}

			friend std::ostream& operator << (std::ostream& os, const Message<T>& msg)
			{
				os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
				return os;
			}

			template<typename DataType>
			friend Message<T>& operator<< (Message<T>& msg, const DataType& data)
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				size_t i = msg.body.size();

				msg.body.resize(msg.body.size() + sizeof(DataType));

				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				msg.header.size = msg.size();

				return msg;
			}

			template<typename DataType>
			friend Message<T>& operator>> (Message<T>& msg, const DataType& data)
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");

				size_t i = msg.body.size() - sizeof(DataType);

				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

				msg.body.resize();

				msg.header.size = msg.size();

				return msg;
			}

		};

	}
}