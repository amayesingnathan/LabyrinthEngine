#pragma once

#include "NetCommon.h"
#include "Message.h"
#include "TSQueue.h"
#include "Connection.h"
#include "NetworkLayer.h"

namespace Labyrinth {

	namespace Net {

		class LAB_API ServerLayer : public NetworkLayer
		{
		public:
			ServerLayer(uint16_t port)
				: mAsioAcceptor(mIOContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
			{
			}

			virtual ~ServerLayer() = default;

		public: //Layer overrides
			virtual void onAttach() override { Start(); }
			virtual void onDetach() override { Stop(); }

			// onUpdate() should generally not be overriden, only onMessage(). onUpdate() will call onMessage() for each message in the incoming queue.
			// onUpdate() can be override if you wish to limit the number of messages read from the queue per game tick. Just call Update(usize maxMessages) within onUpdate().
			virtual void onUpdate(Timestep ts) override { Update(); } 

		protected:
			bool Start()
			{
				try
				{
					WaitForClientConnection();

					mIOContextThread = std::thread([this]() { mIOContext.run(); });
				}
				catch (std::exception& e)
				{
					LAB_CORE_ERROR("Exception: {0}", e.what());
					return false;
				}

				LAB_CORE_TRACE("Server Started!");
				return true;

			}

			void Stop()
			{
				mIOContext.stop();

				if (mIOContextThread.joinable()) mIOContextThread.join();

				LAB_CORE_TRACE("Server Stopped!");
			}

			// Generic update function. Reads all waiting messages and calls onMessage for each message.
			void Update(usize maxMessages = std::numeric_limits<usize>::max())
			{
				usize messageCount = 0;
				while (messageCount < maxMessages && !mQMessagesIn.empty())
				{
					auto msg = mQMessagesIn.pop_front();

					onMessage(msg.remote, msg.msg);

					messageCount++;
				}
			}

		public: // ASYNC
			void WaitForClientConnection()
			{
				mAsioAcceptor.async_accept(
					[this](std::error_code ec, asio::ip::tcp::socket socket)
					{
						if (!ec)
						{
							LAB_CORE_TRACE("New Connection {0}", socket.remote_endpoint());

							Ref<Connection> newConn = Ref<Connection>::Create(Connection::Owner::Server, mIOContext, std::move(socket), mQMessagesIn);

							if (onClientConnect(newConn))
							{
								mConnections.push_back(std::move(newConn));
								mConnections.back()->connectToClient(this, mIDCounter++);

								LAB_CORE_TRACE("[{0}] Connection Approved", mConnections.back()->getID());
							}
							else
							{
								LAB_CORE_INFO("[-----] Connection Denied");
							}
						}
						else
						{
							LAB_CORE_ERROR("New Connection Error {0}", ec.message());
						}

						WaitForClientConnection();
					}
				);
			}

			void MessageClient(Ref<Connection> client, const Message& msg)
			{
				if (client && client->isConnected())
				{
					client->send(msg);
				}
				else
				{
					onClientDisconnect(client);
					client.reset();
					mConnections.erase(std::remove(mConnections.begin(), mConnections.end(), client), mConnections.end());
				}
			}

			void BroadcastToClients(const Message& msg, Ref<Connection> ignoreClient = nullptr)
			{
				bool invalidClientExists = false;

				for (auto& client : mConnections)
				{
					if (client && client->isConnected())
					{
						if (client != ignoreClient)
						{
							client->send(msg);
						}
					}
					else
					{
						onClientDisconnect(client);
						client.reset();
						invalidClientExists = true;
					}
				}

				if (invalidClientExists)
				{
					mConnections.erase(std::remove(mConnections.begin(), mConnections.end(), nullptr), mConnections.end());
				}
			}

		public: // To be overriden by child classes.
			virtual bool onClientConnect(Ref<Connection> client) = 0;
			virtual void onClientDisconnect(Ref<Connection> client) = 0;

			virtual void onClientValidated(Ref<Connection> client) {} // Not a mandatory override but recommended.

		protected:
			TSQueue<OwnedMessage> mQMessagesIn;

			std::deque<Ref<Connection>> mConnections;

			asio::io_context mIOContext;
			std::thread mIOContextThread;

			asio::ip::tcp::acceptor mAsioAcceptor;

			u32 mIDCounter = 10000;
		};

	}
}