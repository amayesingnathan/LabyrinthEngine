#pragma once

#include <Labyrinth/Containers/TSQueue.h>

#include "NetCommon.h"
#include "OwnedMessage.h"
#include "Connection.h"
#include "NetworkLayer.h"

namespace Laby {

	class ServerLayer : public NetworkLayer
	{
	public:
		ServerLayer(uint16_t port);

		virtual ~ServerLayer() = default;

	public: //Layer overrides
		void OnAttach() override { Start(); }
		void OnDetach() override { Stop(); }

		// OnUpdate() should generally not be overriden, but can override if you wish to limit the number
		// of messages read from the queue per game tick. Just call Update(usize maxMessages) within onUpdate().
		void OnUpdate(slc::Timestep ts) override { Update(); }
		void OnEvent(slc::Event& e) override {}
		void OnRender() override {}

		LISTENING_EVENTS(None)

	protected:
		bool Start();
		void Stop();

		// Generic update function. Reads all waiting messages and post a message event for each message.
		void Update(usize maxMessages = std::numeric_limits<usize>::max());

	public: // ASYNC
		void WaitForClientConnection();
		void MessageClient(UUID clientID, const Message& msg);
		void BroadcastToClients(const Message& msg, UUID ignoreClient = 0);

	private:
		void MessageEachClient(Ref<Connection> client, const Message& msg, UUID ignoreClient);

	public: // To be overriden by child classes.
		virtual bool OnClientConnect(Ref<Connection> client) = 0;
		virtual void OnClientDisconnect(Ref<Connection> client) {}

		virtual void OnClientValidated(Ref<Connection> client) {}

	protected:
		TSQueue<OwnedMessage> mQMessagesIn;

		std::vector<Ref<Connection>> mConnections;

		asio::io_context mIOContext;
		std::thread mIOContextThread;

		asio::ip::tcp::acceptor mAsioAcceptor;

		u32 mIDCounter = 10000;
	};

	template<typename T>
	concept IsServer = DerivedFrom<T, ServerLayer>;
}