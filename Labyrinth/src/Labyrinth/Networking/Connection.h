#pragma once

#include "NetCommon.h"
#include "TSQueue.h"
#include "Message.h"

namespace Labyrinth {

	namespace Net {

		template<typename T>
		class Connection : public RefFromThis<Connection<T>>
		{
		public:
			enum class Owner
			{
				Server,
				Client
			};

		public:
			Connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, TSQueue<OwnedMessage<T>>& qIn) 
				: mAsioContext(asioContext), mSocket(std::move(socket)), mQMessagesIn(qIn)
			{
				mOwnerType = parent;
			}

			virtual ~Connection() 
			{
			}

			uint32_t getID() const { return mID; }

		public:
			void ConnectToClient(uint32_t uid = 0)
			{
				if (mOwnerType != Owner::Server) return;
				if (!mSocket.is_open()) return;

				mID = uid;
				ReadHeader();
			}

			bool ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (mOwnerType != Owner::Client) return;

				asio::async_connect(mSocket, endpoints,
					[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
					{
						if (!ec)
						{
							ReadHeader();
						}
						else
						{
							std::cout << "[" << mID << "] Failed to connect to server.\n";
						}
					}
				)
			}

			bool Disconnect()
			{
				if (IsConnected())
					asio::post(mAsioContext, [this]() { mSocket.close(); });
			}

			bool IsConnected() const
			{
				return mSocket.is_open();
			}

		public:
			bool Send(const Message<T>& msg)
			{
				asio::post(mAsioContext,
					[this, msg]()
					{
						//If the messages out queue is not empty, then messages are already being written...
						bool currentlyWriting = !mQMessagesOut.empty();
						mQMessagesOut.push_back(msg);
						if (!currentlyWriting)
							WriteHeader();  // ...so we only want to start writing messages if it isn't already doing so
					});
			}

		private:
			// ASYNC
			void ReadHeader()
			{
				asio::async_read(mSocket, asio::buffer(&mMsgTemporaryIn.header, sizeof(MessageHeader<T>)),
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
							{
								addToIncomingMessageQueue();
							}
						}
						else
						{
							std::cout << "[" << mID << "] Read Header Fail.\n";
							mSocket.close();
						}
					}
			}

			// ASYNC
			void ReadBody()
			{
				asio::async_read(mSocket, asio::buffer(&mMsgTemporaryIn.body.data(), mMsgTemporaryIn.body.size),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							addToIncomingMessageQueue();
						}
						else
						{
							std::cout << "[" << mID << "] Read Body Fail.\n";
							mSocket.close();
						}
					}
			}

			// ASYNC
			void WriteHeader()
			{
				asio::async_write(mSocket, asio::buffer(&mQMessagesOut.front().header, sizeof(MessageHeader<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (mQMessagesOut.front().body.size() > 0)
							{
								WriteBody();
							}
							else
							{
								mQMessagesOut.pop_front();

								if (!mQMessagesOut.empty())
									WriteHeader();
							}
						}
						else
						{
							std::cout << "[" << mID << "] Write Header Fail.\n";
							mSocket.close();
						}
					}
			}

			// ASYNC
			void WriteBody()
			{
				asio::async_write(mSocket, asio::buffer(&mQMessagesOut.front().body.data(), &mQMessagesOut.front().body.size),
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
							std::cout << "[" << mID << "] Write Body Fail.\n";
							mSocket.close();
						}
					}
			}

			// ASYNC
			void AddToIncomingMessageQueue()
			{
				if (mOwnerType == Owner::Server)
					mQMessagesIn.push_back({ this->shared_from_this(), mMsgTemporaryIn });
				else
					mQMessagesIn.push_back({ nullptr, mMsgTemporaryIn });

				ReadHeader();
			}

		protected:
			Owner mOwnerType = Owner::Server;
			uint32_t mID = 0;

			asio::ip::tcp::socket mSocket;
			asio::io_context& mAsioContext;

			TSQueue<Message<T>> mQMessagesOut;
			TSQueue<OwnedMessage<T>>& mQMessagesIn;

			Message<T> mMsgTemporaryIn;
		};

	}

}