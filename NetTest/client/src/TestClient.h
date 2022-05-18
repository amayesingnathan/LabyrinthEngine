#pragma once
#include <Labyrinth.h>

#include "Labyrinth/Networking/ClientLayer.h"

namespace Labyrinth {

	namespace Net {

		class TestClientLayer : public ClientLayer<MessageType>
		{
		public:

		protected:  //Layer overrides
			virtual void onUpdate(Timestep ts) override;
			virtual void onImGuiRender() override {}
			virtual void onEvent(Event& e) override;

		private:
			bool OnKeyPressed(KeyPressedEvent& e);

			void PingServer();
			void MessageAll();

		private:
			Timer mTimer;

		};
	}
}