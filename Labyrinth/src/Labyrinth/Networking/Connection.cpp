#include "Lpch.h"
#include "Connection.h"

#include "ServerLayer.h"

namespace Labyrinth {

	namespace Net {
		void Connection::WriteHeader()
		{
			asio::async_write(mSocket, asio::buffer(&mQMessagesOut.front().header, sizeof(MessageHeader)),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						if (mQMessagesOut.front().body.size() > 0) 
							WriteBody();
						else
						{
							mQMessagesOut.pop_front();

							if (!mQMessagesOut.empty())
								WriteHeader();
						}
					}
					else
					{
						LAB_CORE_WARN("[{0}] Write Header Fail.", mID);
						mSocket.close();
					}
				});
		}

		void Connection::WriteBody()
		{
			asio::async_write(mSocket, asio::buffer(mQMessagesOut.front().body.data(), mQMessagesOut.front().body.size()),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						mQMessagesOut.pop_front();

						if (!mQMessagesOut.empty())
							WriteHeader();
					}
					else
					{
						LAB_CORE_WARN("[{0}] Write Body Fail.", mID);
						mSocket.close();
					}
				});
		}

		void Connection::ReadHeader()
		{
			asio::async_read(mSocket, asio::buffer(&mMsgTemporaryIn.header, sizeof(MessageHeader)),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						if (mMsgTemporaryIn.header.size > 0)
						{
							mMsgTemporaryIn.body.resize(mMsgTemporaryIn.header.size);
							ReadBody();
						}
						else
							AddToIncomingMessageQueue();
					}
					else
					{
						LAB_CORE_WARN("[{0}] Read Header Fail.", mID);
						mSocket.close();
					}
				});
		}

		void Connection::ReadBody()
		{
			asio::async_read(mSocket, asio::buffer(mMsgTemporaryIn.body.data(), mMsgTemporaryIn.body.size()),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
						AddToIncomingMessageQueue();
					else
					{
						LAB_CORE_WARN("[{0}] Read Body Fail.", mID);
						mSocket.close();
					}
				});
		}

		void Connection::AddToIncomingMessageQueue()
		{
			if (mOwnerType == Owner::Server)
				mQMessagesIn.push_back({ CreateRef(this), mMsgTemporaryIn });
			else
				mQMessagesIn.push_back({ nullptr, mMsgTemporaryIn });

			ReadHeader();
		}

		void Connection::WriteValidation()
		{
			asio::async_write(mSocket, asio::buffer(&mHandshakeOut, sizeof(uint64_t)),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						if (mOwnerType == Owner::Client)
							ReadHeader();
					}
					else
					{
						LAB_CORE_WARN("[{0}] Write Validation Fail.", mID);
						mSocket.close();
					}
				});
		}

		void Connection::ReadValidation(ServerLayer* server)
		{
			asio::async_read(mSocket, asio::buffer(&mHandshakeIn, sizeof(uint64_t)),
				[this, server](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						if (mOwnerType == Owner::Server)
						{
							if (mHandshakeIn == mHandshakeCheck)
							{
								LAB_CORE_INFO("Client Validated");
								server->onClientValidated(CreateRef(this));

								ReadHeader();
							}
							else
							{
								LAB_CORE_WARN("Client Disconnected (Failed Validation)");
								mSocket.close();
							}
						}
						else //This is a client
						{
							mHandshakeOut = Scramble(mHandshakeIn);
							WriteValidation();
						}
					}
					else
					{
						LAB_CORE_WARN("[{0}] Read Validation Fail.", mID);
						mSocket.close();
					}
				});
		}
	}
}