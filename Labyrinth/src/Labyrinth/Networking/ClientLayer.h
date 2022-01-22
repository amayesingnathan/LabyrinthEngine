#pragma once

#include "NetCommon.h"
#include "Message.h"
#include "TSQueue.h"
#include "Connection.h"

namespace Labyrinth {

	namespace Net {

		template<typename T>
		class ClientLayer : public Layer
		{
		public:
			ClientLayer() {}

			virtual ~ClientLayer()
			{
				if (isConnected())
					disconnect();
			}

		public: //Layer overrides
			virtual void onAttach() override
			{
				connect("127.0.0.1", 60000);
			}
			virtual void onDetach() override 
			{
				disconnect();
			}

		public:
			bool connect(const std::string& host, const uint16_t port)
			{
				try
				{
					asio::ip::tcp::resolver resolver(mIOContext);
					asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

					mConnection = CreateSingle<Connection<T>>(Connection<T>::Owner::Client, mIOContext, asio::ip::tcp::socket(mIOContext), mQMessagesIn);

					mConnection->connectToServer(endpoints);

					mIOContextThread = std::thread([this]() { mIOContext.run(); });
				}
				catch (std::exception& e)
				{
					LAB_CORE_ERROR("Client Exception: {0}", e.what());
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

				mIOContext.stop();

				if (mIOContextThread.joinable())
					mIOContextThread.join();

				mConnection.release();
			}

			bool isConnected() const
			{
				if (mConnection)
					return mConnection->isConnected();
				else
					return false;
			}

			TSQueue<OwnedMessage<T>>& incoming()
			{
				return mQMessagesIn;
			}

		public:
			void Send(const Message<T>& msg)
			{
				if (isConnected())
					mConnection->send(msg);
			}

		protected:
			asio::io_context mIOContext;
			std::thread mIOContextThread;

			Single<Connection<T>> mConnection;

		private:
			TSQueue<OwnedMessage<T>> mQMessagesIn;
		};

	}

}