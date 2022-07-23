#pragma once

#include "NetCommon.h"
#include "TSQueue.h"
#include "Message.h"

namespace Labyrinth {

	namespace Net {

		class ServerLayer;

		class LAB_API Connection : public RefCounted
		{
		public:
			enum class Owner
			{
				Server,
				Client
			};

		public:
			Connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, TSQueue<OwnedMessage>& qIn)
				: mIOContext(asioContext), mSocket(std::move(socket)), mQMessagesIn(qIn)
			{
				mOwnerType = parent;

				if (mOwnerType == Owner::Server)
				{
					mHandshakeOut = u64(std::chrono::system_clock::now().time_since_epoch().count());

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

			u32 getID() const { return mID; }

		public:
			void connectToClient(ServerLayer* server, u32 uid = 0)
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
							ReadValidation();
						else
							LAB_CORE_WARN("[{0}] Failed to connect to server.", mID);
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
			void send(const Message& msg)
			{
				asio::post(mIOContext,
					[this, msg]()
					{
						//If the messages out queue is not empty, then messages are already being written...
						bool currentlyWriting = !mQMessagesOut.empty();
						mQMessagesOut.push_back(msg);

						if (!currentlyWriting) WriteHeader();  // ...so we only want to start writing messages if it isn't already doing so 
					});
			}

		private:
			// ASYNC
			void WriteHeader();

			// ASYNC
			void WriteBody();

			// ASYNC
			void ReadHeader();

			// ASYNC
			void ReadBody();

			// ASYNC
			void AddToIncomingMessageQueue();

			u64 Scramble(u64 input)
			{
				u64 out = input ^ 0xA86DEBA86581;
				constexpr i32 ver = Labyrinth::Cast<i32>(LABYRINTH_VER * 100);
				out = (out & ver) >> 4 | (out & ver) << 4;
				return out ^ 0xA88DABE51581;
			}

			void WriteValidation();
			void ReadValidation(Labyrinth::Net::ServerLayer* server = nullptr);

		protected:
			Owner mOwnerType = Owner::Server;
			u32 mID = 0;

			asio::ip::tcp::socket mSocket;
			asio::io_context& mIOContext;

			TSQueue<Message> mQMessagesOut;
			TSQueue<OwnedMessage>& mQMessagesIn;

			Message mMsgTemporaryIn;

			u64 mHandshakeIn = 0;
			u64 mHandshakeOut = 0;
			u64 mHandshakeCheck = 0;
		};
	}
}