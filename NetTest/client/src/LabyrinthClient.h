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

		class TestClientLayer : public ClientLayer<MessageTypes>
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