#pragma once

#include "NetCommon.h"
#include "Message.h"
#include "TSQueue.h"
#include "Connection.h"

namespace Labyrinth {

	namespace Net {

		template<typename T>
		class ClientInterface
		{
		public:
			ClientInterface() : mSocket(mContext)
			{
				// Initialise the socket with the io context.
			}

			virtual ~ClientInterface()
			{
				disconnect();
			}

			bool connect(const std::string& host, const unt16_t port)
			{
				try
				{
					asio::ip::tcp::resolver resolver(mContext);
					asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

					mConnection = CreateSingle<Connection<T>>(Connection<T>::Owner::Client, mContext, asio::ip::tcp::socket(mContext), mQMessagesIn);

					mConnection->connectToServer(endpoints);

					mContextThread = std::thread([this]() { mContext.run(); });
				}
				catch (std::exception& e)
				{
					std::cerr << "Client Exception: " << e.what() << "\n";
					return false;
				}

				return false;
			}

			void disconnect()
			{
				if (isConnected())
				{
					mConnection->disconnect();
				}

				mContext.stop();

				if (mContextThread.joinable())
					mContextThread.join();

				mConnection.release();
			}

			bool isConnected() const
			{
				return false;
			}

			TSQueue<OwnedMessage<T>>& incoming()
			{
				return mQMessagesIn;
			}

		protected:
			asio::io_context mContext;
			std::thread mContextThread;

			asio::ip::tcp::socket mSocket;

			Single<Connection<T>> mConnection;

		private:
			TSQueue<OwnedMessage<T>> mQMessagesIn;
		};

	}

}