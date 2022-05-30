#pragma once

#include "NetCommon.h"
#include "TSQueue.h"
#include "Message.h"

namespace Labyrinth {

	namespace Net {

		class ServerLayer;

		class LAB_API Connection : public AllowRefFromThis<Connection>
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
			void connectToClient(ServerLayer* server, uint32_t uid = 0)
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

			uint64_t Scramble(uint64_t input)
			{
				uint64_t out = input ^ 0xA86DEBA86581;
				constexpr int ver = Labyrinth::Cast<int>(LABYRINTH_VER * 100);
				out = (out & ver) >> 4 | (out & ver) << 4;
				return out ^ 0xA88DABE51581;
			}

			void WriteValidation();
			void ReadValidation(Labyrinth::Net::ServerLayer* server = nullptr);

		protected:
			Owner mOwnerType = Owner::Server;
			uint32_t mID = 0;

			asio::ip::tcp::socket mSocket;
			asio::io_context& mIOContext;

			TSQueue<Message> mQMessagesOut;
			TSQueue<OwnedMessage>& mQMessagesIn;

			Message mMsgTemporaryIn;

			uint64_t mHandshakeIn = 0;
			uint64_t mHandshakeOut = 0;
			uint64_t mHandshakeCheck = 0;
		};
	}
}