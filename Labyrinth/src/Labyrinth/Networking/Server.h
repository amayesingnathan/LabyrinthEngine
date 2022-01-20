#pragma once

#include "NetCommon.h"
#include "Message.h"
#include "TSQueue.h"
#include "Connection.h"

namespace Labyrinth {

	namespace Net {

		template<typename T>
		class ServerInterface
		{
		public:
			ServerInterface(uint16_t port) 
				: mAsioAcceptor(mAsioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
			{
			}

			virtual ~ServerInterface() 
			{
				stop();
			}

			bool start()
			{
				try
				{
					waitForClientConnection();

					mContextThread = std::thread([this]() { mAsioContext.run(); });
				}
				catch (std::exception& e)
				{
					std::cerr << "[SERVER] Exception: " << e.what() << "\n";
					return false;
				}

				std::cout << "[SERVER] Started!\n";
				return true;

			}

			void stop()
			{
				mAsioContext.stop();

				if (mContextThread.joinable()) mContextThread.join();
				std::cout << "[SERVER] Stopped!\n";
			}

			// ASYNC
			void waitForClientConnection()
			{
				mAsioAcceptor.async_accept(
					[this](std::error_code ec, asio::ip::tcp::socket socket)
					{
						if (!ec)
						{
							std::cout << "[SERVER] New Connection " << socket.remote_endpoint() << "\n";

							Ref<Connection<T>> newConn = CreateRef<Connection<T>>(Connection<T>::owner::server, mAsioContext, std::move(socket), mQMessagesIn);

							if (onClientConnect(newConn))
							{
								mConnections.push_back(std::move(newConn));
								mConnections.back()->connectToClient(mIDCounter++);

								std::cout << "[" << mConnections.back()->getID() << "] Connection Approved\n";
							}
							else
							{
								std::cout << "[-----] Connection Denied\n";
							}
						}
						else
						{
							std::cout << "[SERVER] New Connection Error " << ec.message() << "\n";
						}

						waitForClientConnection();
					}
				)
			}

			void messageClient(Ref<Connection<T>> client, const Message<T> msg)
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

			void broadcastToClients(const Message<T> msg, Ref<Connection<T>> ignoreClient = nullptr)
			{
				bool invalidClientExists = false;;

				for (auto& client : mConnections)
				{
					if (client && client->isConnected())
					{
						if (client != ignoreClient)
							client->send(msg);
					}
					else
					{
						onClientDisconnect(client);
						client.reset();
						invalidClientExists = true;
					}
				}

				if (invalidClientExists)
					mConnections.erase(std::remove(mConnections.begin(), mConnections.end(), nullptr), mConnections.end());
			}

			void update(size_t maxMessages = -1)
			{
				size_t messageCount = 0;
				while (messageCount < maxMessages && !mQMessagesIn.empty())
				{
					auto msg = mQMessagesIn.pop_front();

					onMessage(msg.remote, msg.msg);

					messageCount++;
				}
			}

		protected:
			virtual bool onClientConnect(Ref<Connection<T>> client)
			{
				return false;
			}

			virtual void onClientDisconnect(Ref<Connection<T>> client)
			{
				std::cout << "Removing client [" << client->getID() << "]\n";
			}

			virtual void onMessage(Ref<Connection<T>> client, Message<T>& msg)
			{

			}

		protected:
			TSQueue<OwnedMessage<T>> mQMessagesIn;

			std::deque<Ref<Connection<T>>> mConnections;

			asio::io_context mAsioContext;
			std::thread mContextThread;

			asio::ip::tcp::acceptor mAsioAcceptor;

			uint32_t mIDCounter = 10000;
		};

	}
}