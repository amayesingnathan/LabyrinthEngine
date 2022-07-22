#pragma once

#include "NetCommon.h"
#include "Message.h"
#include "TSQueue.h"
#include "Connection.h"
#include "NetworkLayer.h"

namespace Labyrinth {

	namespace Net {

		class LAB_API ClientLayer : public NetworkLayer
		{
		public:
			ClientLayer() {}

			virtual ~ClientLayer()
			{
				if (isConnected())
					Disconnect();
			}

			// Override and call Connect() to use different connection than localhost:60000.
			virtual void onAttach() override { Connect("127.0.0.1", 60000); }
			virtual void onDetach() override { Disconnect(); }

			// onUpdate() should generally not be overriden, only onMessage(). onUpdate() will call onMessage() for each message in the incoming queue.
			// onUpdate() can be override if you wish to limit the number of messages read from the queue per game tick. Just call Update(usize maxMessages) within onUpdate().
			virtual void onUpdate(Timestep ts) override { Update(); }

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

		private:
			// Generic update function. Reads all waiting messages and calls onMessage for each message.
			void Update(usize maxMessages = std::numeric_limits<usize>::max())
			{
				if (!isConnected()) return;

				usize messageCount = 0;
				while (messageCount < maxMessages && !mQMessagesIn.empty())
				{
					auto msg = mQMessagesIn.pop_front();

					onMessage(msg.remote, msg.msg);

					messageCount++;
				}
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