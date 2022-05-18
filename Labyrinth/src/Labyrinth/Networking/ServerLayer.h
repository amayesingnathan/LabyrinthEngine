#pragma once

#include "NetCommon.h"
#include "Message.h"
#include "TSQueue.h"
#include "Connection.h"
#include "NetworkLayer.h"

namespace Labyrinth {

	namespace Net {

		template<typename T>
		class ServerLayer : public NetworkLayer
		{
		public:
			ServerLayer(uint16_t port)
				: mAsioAcceptor(mIOContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
			{
			}

			virtual ~ServerLayer() = default;

		public: //Layer overrides
			virtual void onAttach() override { start(); }
			virtual void onDetach() override { stop(); }

			virtual void onUpdate(Timestep ts) override
			{
				update(-1, true);
			}

		public:
			bool start()
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

			void stop()
			{
				mIOContext.stop();

				if (mIOContextThread.joinable()) mIOContextThread.join();

				LAB_CORE_TRACE("Server Stopped!");
			}

			// ASYNC
			void WaitForClientConnection()
			{
				mAsioAcceptor.async_accept(
					[this](std::error_code ec, asio::ip::tcp::socket socket)
					{
						if (!ec)
						{
							LAB_CORE_TRACE("New Connection {0}", socket.remote_endpoint());

							Ref<Connection<T>> newConn = CreateRef<Connection<T>>(Connection<T>::Owner::Server, mIOContext, std::move(socket), mQMessagesIn);

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

			void MessageClient(Ref<Connection<T>> client, const Message<T>& msg)
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

			void BroadcastToClients(const Message<T>& msg, Ref<Connection<T>> ignoreClient = nullptr)
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

			void update(size_t maxMessages = std::numeric_limits<size_t>::max(), bool wait = false)
			{
				if (wait) mQMessagesIn.wait();

				size_t messageCount = 0;
				while (messageCount < maxMessages && !mQMessagesIn.empty())
				{
					auto msg = mQMessagesIn.pop_front();

					onMessage(msg.remote, msg.msg);

					messageCount++;
				}
			}

		protected: // To be overriden by child classes.
			virtual bool onClientConnect(Ref<Connection<T>> client)
			{
				return false;
			}

			virtual void onClientDisconnect(Ref<Connection<T>> client)
			{
				LAB_CORE_TRACE("Removing client [{0}]", client->getID());
			}

			virtual void onMessage(Ref<Connection<T>> client, Message<T>& msg)
			{

			}

		public:
			virtual void onClientValidated(Ref<Connection<T>> client)
			{

			}

		protected:
			TSQueue<OwnedMessage<T>> mQMessagesIn;

			std::deque<Ref<Connection<T>>> mConnections;

			asio::io_context mIOContext;
			std::thread mIOContextThread;

			asio::ip::tcp::acceptor mAsioAcceptor;

			uint32_t mIDCounter = 10000;
		};

	}
}