#pragma once
#include <Labyrinth.h>

#include "Labyrinth/Networking/ClientLayer.h"

namespace Labyrinth {

	namespace Net {

		class TestClientLayer : public ClientLayer
		{
		public:

		protected: 
			virtual void onImGuiRender() override {}
			virtual void onEvent(Event& e) override;

			virtual void onMessage(Ref<Connection> server, Message& msg) override;

		private:
			bool OnKeyPressed(KeyPressedEvent& e);

			void PingServer();
			void MessageAll();

		private:
			Timer mTimer;

		};
	}
}