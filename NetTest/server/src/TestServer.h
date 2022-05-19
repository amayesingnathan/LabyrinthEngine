#pragma once
#include <Labyrinth.h>

#include "Labyrinth/Networking/ServerLayer.h"

namespace Labyrinth {

	namespace Net {

		class TestServerLayer : public ServerLayer
		{
		public:
			TestServerLayer(uint16_t port) : ServerLayer(port) {}

		protected:  //Layer overrides
			virtual void onEvent(Labyrinth::Event& e) override {}

		protected: //Server Interface overrides
			virtual bool onClientConnect(Ref<Connection> client) override;

			virtual void onClientDisconnect(Ref<Connection> client) override
			{
				LAB_INFO("Removing client [{0}]", client->getID());
			}

			virtual void onMessage(Ref<Connection> client, Message& msg) override;
		};
	}
}