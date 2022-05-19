#include "TestClient.h"

namespace Labyrinth
{
	namespace Net {

		void TestClientLayer::onUpdate(Timestep ts)
		{
			if (isConnected())
			{
				if (!incoming().empty())
				{
					auto msg = incoming().pop_front().msg;

					switch (msg.header.id)
					{
					case MessageTypes::ServerAccept:
					{
						LAB_INFO("Server Accepted Connection");
					}
					break;

					case MessageTypes::ServerPing:
					{
						LAB_INFO("Pinged Server: {0}ms", mTimer.elapsedMillis());
					}
					break;

					case MessageTypes::ServerMessage:
					{
						uint32_t clientID;
						msg >> clientID;
						LAB_INFO("Hello from [{0}]", clientID);
					}
					break;
					}
				}
			}
		}

		void TestClientLayer::onEvent(Event& e)
		{
			EventDispatcher dispatcher(e);
			dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(TestClientLayer::OnKeyPressed));
		}

		bool TestClientLayer::OnKeyPressed(KeyPressedEvent& e)
		{
			if (!isConnected()) { LAB_INFO("Server is down"); return false; }

			switch (e.getKeyCode())
			{
			case Key::D1: PingServer(); break;
			case Key::D2: MessageAll(); break;
			default: break;
			}

			return false;
		}



		void TestClientLayer::PingServer()
		{
			Message msg;
			msg.header.id = MessageTypes::ServerPing;

			mTimer.reset();
			Send(msg);
		}

		void TestClientLayer::MessageAll()
		{
			Message msg;
			msg.header.id = MessageTypes::MessageAll;

			Send(msg);
		}
	}
}