#pragma once

#include "NetCommon.h"
#include "TSQueue.h"
#include "Message.h"

namespace Labyrinth {

	namespace Net {

		template<typename T>
		class ServerLayer;

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
				: mIOContext(asioContext), mSocket(std::move(socket)), mQMessagesIn(qIn)
			{
				mOwnerType = parent;

				if (mOwnerType == Owner::Server)
				{
					mHandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());

					mHandshakeCheck = Scramble(mHandshakeOut);
				}
				else
				{
					mHandshakeOut = 0;
					mHandshakeIn = 0;
				}
			}

			virtual ~Connection() 
			{
			}

			uint32_t getID() const { return mID; }

		public:
			void connectToClient(ServerLayer<T>* server, uint32_t uid = 0)
			{
				if (mOwnerType != Owner::Server) return;
				if (!mSocket.is_open()) return;

				mID = uid;

				WriteValidation();
				ReadValidation(server);
			}

			void connectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (mOwnerType != Owner::Client) return;

				asio::async_connect(mSocket, endpoints,
					[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
					{
						if (!ec)
						{
							ReadValidation();
						}
						else
						{
							LAB_CORE_WARN("[{0}] Failed to connect to server.", mID);
						}
					});
			}

			void disconnect()
			{
				if (isConnected())
					asio::post(mIOContext, [this]() { mSocket.close(); });
			}

			bool isConnected() const
			{
				return mSocket.is_open();
			}

		public:
			// ASYNC
			void send(const Message<T>& msg)
			{
				asio::post(mIOContext,
					[this, msg]()
					{
						//If the messages out queue is not empty, then messages are already being written...
						bool currentlyWriting = !mQMessagesOut.empty();
						mQMessagesOut.push_back(msg);
						if (!currentlyWriting)
						{
							WriteHeader();  // ...so we only want to start writing messages if it isn't already doing so
						}
					});
			}

		private:
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
								{
									WriteHeader();
								}
							}
						}
						else
						{
							LAB_CORE_WARN("[{0}] Write Header Fail.", mID);
							mSocket.close();
						}
					});
			}

			// ASYNC
			void WriteBody()
			{
				asio::async_write(mSocket, asio::buffer(mQMessagesOut.front().body.data(), mQMessagesOut.front().body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							mQMessagesOut.pop_front();

							if (!mQMessagesOut.empty())
							{
								WriteHeader();
							}
						}
						else
						{
							LAB_CORE_WARN("[{0}] Write Body Fail.", mID);
							mSocket.close();
						}
					});
			}
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
								AddToIncomingMessageQueue();
							}
						}
						else
						{
							LAB_CORE_WARN("[{0}] Read Header Fail.", mID);
							mSocket.close();
						}
					});
			}

			// ASYNC
			void ReadBody()
			{
				asio::async_read(mSocket, asio::buffer(mMsgTemporaryIn.body.data(), mMsgTemporaryIn.body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							AddToIncomingMessageQueue();
						}
						else
						{
							LAB_CORE_WARN("[{0}] Read Body Fail.", mID);
							mSocket.close();
						}
					});
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

			uint64_t Scramble(uint64_t input)
			{
				uint64_t out = input ^ 0xA86DEBA86581;
				int ver = Labyrinth::Cast<int>(LABYRINTH_VER * 100);
				out = (out & ver) >> 4 | (out & ver) << 4;
				return out ^ 0xA88DABE51581;
			}

			void WriteValidation()
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

			void ReadValidation(Labyrinth::Net::ServerLayer<T>* server = nullptr)
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
									server->onClientValidated(this->shared_from_this());

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

		protected:
			Owner mOwnerType = Owner::Server;
			uint32_t mID = 0;

			asio::ip::tcp::socket mSocket;
			asio::io_context& mIOContext;

			TSQueue<Message<T>> mQMessagesOut;
			TSQueue<OwnedMessage<T>>& mQMessagesIn;

			Message<T> mMsgTemporaryIn;

			uint64_t mHandshakeIn = 0;
			uint64_t mHandshakeOut = 0;
			uint64_t mHandshakeCheck = 0;
		};

	}

}