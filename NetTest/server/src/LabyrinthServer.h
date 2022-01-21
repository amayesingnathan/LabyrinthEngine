#pragma once
#include <Labyrinth.h>

namespace Labyrinth {

	namespace Net {

		enum class MessageTypes : uint32_t
		{
			ServerAccept,
			ServerDeny,
			ServerPing,
			MessageAll,
			ServerMessage,
		};

		class TestServerLayer : public ServerLayer<MessageTypes>
		{
		public:
			TestServerLayer(uint16_t port) : ServerLayer<MessageTypes>(port) {}

		protected:  //Layer overrides

			virtual void onUpdate(Timestep ts) override
			{
				update(-1, true);
			}

			virtual void onEvent(Labyrinth::Event& e) override {}

		protected: //Server Interface overrides
			virtual bool onClientConnect(Ref<Connection<MessageTypes>> client) override;

			virtual void onClientDisconnect(Ref<Connection<MessageTypes>> client) override
			{
				LAB_INFO("Removing client [{0}]", client->getID());
			}

			virtual void onMessage(Ref<Connection<MessageTypes>> client, Message<MessageTypes>& msg) override;
		};
	}
}