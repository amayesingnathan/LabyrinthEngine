#include "LabyrinthServer.h"

namespace Labyrinth
{
	namespace Net {

		void TestServerLayer::onUpdate(Timestep ts)
		{
			update(-1, true);
		}

		bool TestServerLayer::onClientConnect(Ref<Connection<MessageTypes>> client)
		{
			Message<MessageTypes> msg;
			msg.header.id = MessageTypes::ServerAccept;
			client->send(msg);
			return true;
		}

		void TestServerLayer::onMessage(Ref<Connection<MessageTypes>> client, Message<MessageTypes>& msg)
		{
			switch (msg.header.id)
			{
			case MessageTypes::ServerPing:
			{
				LAB_INFO("[{0}]: Server Ping", client->getID());

				// Simply bounce message back to client
				client->send(msg);
			}
			break;

			case MessageTypes::MessageAll:
			{
				LAB_INFO("[{0}]: Message All", client->getID());

				// Construct a new message and send it to all clients
				Message<MessageTypes> msg;
				msg.header.id = MessageTypes::ServerMessage;
				msg << client->getID();
				BroadcastToClients(msg, client);

			}
			break;
			}
		}
	}
}