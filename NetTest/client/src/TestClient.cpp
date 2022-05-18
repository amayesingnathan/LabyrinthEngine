#include "TestClient.h"

namespace Labyrinth
{
	namespace Net {

		void TestClientLayer::onUpdate(Timestep ts)
		{
			bool key[3] = { false, false, false };
			bool old_key[3] = { false, false, false };

			bool bQuit = false;
			while (!bQuit)
			{
				if (GetForegroundWindow() == GetConsoleWindow())
				{
					key[0] = GetAsyncKeyState('1') & 0x8000;
					key[1] = GetAsyncKeyState('2') & 0x8000;
					key[2] = GetAsyncKeyState('3') & 0x8000;
				}

				if (key[0] && !old_key[0])
					PingServer();
				if (key[1] && !old_key[1])
					MessageAll();
				if (key[2] && !old_key[2]) bQuit = true;

				for (int i = 0; i < 3; i++) old_key[i] = key[i];

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
			Message<MessageTypes> msg;
			msg.header.id = MessageTypes::ServerPing;

			mTimer.reset();
			Send(msg);
		}

		void TestClientLayer::MessageAll()
		{
			Message<MessageTypes> msg;
			msg.header.id = MessageTypes::MessageAll;

			Send(msg);
		}
	}
}