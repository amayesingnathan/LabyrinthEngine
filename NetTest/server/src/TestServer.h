#pragma once
#include <Labyrinth.h>

#include "Labyrinth/Networking/ServerLayer.h"

namespace Labyrinth {

	namespace Net {

		class TestServerLayer : public ServerLayer<MessageType>
		{
		public:
			TestServerLayer(uint16_t port) : ServerLayer<MessageType>(port) {}

		protected:  //Layer overrides

			virtual void onUpdate(Timestep ts) override;
			virtual void onEvent(Labyrinth::Event& e) override {}

		protected: //Server Interface overrides
			virtual bool onClientConnect(Ref<Connection<MessageType>> client) override;

			virtual void onClientDisconnect(Ref<Connection<MessageType>> client) override
			{
				LAB_INFO("Removing client [{0}]", client->getID());
			}

			virtual void onMessage(Ref<Connection<MessageType>> client, Message<MessageType>& msg) override;
		};
	}
}