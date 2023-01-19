#pragma once

#include <Labyrinth/Containers/TSQueue.h>

#include "NetCommon.h"
#include "OwnedMessage.h"
#include "Connection.h"
#include "NetworkLayer.h"

namespace Laby {

	class ClientLayer : public NetworkLayer
	{
	public:
		ClientLayer() {}
		virtual ~ClientLayer();

		// Override and call Connect() to use different connection than localhost:60000.
		virtual void onAttach() override { Connect("127.0.0.1", 60000); }
		void onDetach() override { Disconnect(); }

		// onUpdate() should generally not be overriden, but can override if you wish to limit the number
		// of messages read from the queue per game tick. Just call Update(usize maxMessages) within onUpdate().
		void onUpdate(Timestep ts) override { Update(); }
		void onEvent(Event& e) override {}

		void send(const Message& msg) override;

	protected:
		void Connect(const std::string& host, const u16 port);
		void Disconnect();

		// Generic update function. Reads all waiting messages and calls onMessage for each message.
		void Update(usize maxMessages = std::numeric_limits<usize>::max());

	public:
		bool isConnected() const;

		TSQueue<OwnedMessage>& incoming() { return mQMessagesIn; }

		LISTENING_EVENTS(None)

	protected:
		asio::io_context mIOContext;
		std::thread mIOContextThread;

		Single<Connection> mConnection;

	private:
		TSQueue<OwnedMessage> mQMessagesIn;
	};
}