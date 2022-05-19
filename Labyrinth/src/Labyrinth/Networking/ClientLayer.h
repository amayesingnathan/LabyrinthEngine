#pragma once

#include "NetCommon.h"
#include "Message.h"
#include "TSQueue.h"
#include "Connection.h"
#include "NetworkLayer.h"

namespace Labyrinth {

	namespace Net {

		class ClientLayer : public NetworkLayer
		{
		public:
			ClientLayer() {}

			virtual ~ClientLayer()
			{
				if (isConnected())
					Disconnect();
			}

		public: //Layer overrides
			virtual void onAttach() override
			{
				Connect("127.0.0.1", 60000);
			}
			virtual void onDetach() override
			{
				Disconnect();
			}

		protected:
			bool Connect(const std::string& host, const uint16_t port)
			{
				try
				{
					asio::ip::tcp::resolver resolver(mIOContext);
					asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

					mConnection = CreateSingle<Connection>(Connection::Owner::Client, mIOContext, asio::ip::tcp::socket(mIOContext), mQMessagesIn);

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

			void Disconnect()
			{
				if (isConnected())
					mConnection->disconnect();

				mIOContext.stop();

				if (mIOContextThread.joinable())
					mIOContextThread.join();

				mConnection.release();
			}

			void Send(const Message& msg)
			{
				if (isConnected())
					mConnection->send(msg);
			}

		public:
			bool isConnected() const
			{
				if (mConnection)
					return mConnection->isConnected();

				return false;
			}

			TSQueue<OwnedMessage>& incoming()
			{
				return mQMessagesIn;
			}

		protected:
			asio::io_context mIOContext;
			std::thread mIOContextThread;

			Single<Connection> mConnection;

		private:
			TSQueue<OwnedMessage> mQMessagesIn;
		};

	}

}