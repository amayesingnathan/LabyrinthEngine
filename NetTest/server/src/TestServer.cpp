#include "TestServer.h"

namespace Labyrinth
{
	namespace Net {

		bool TestServerLayer::onClientConnect(Ref<Connection> client)
		{
			Message msg;
			msg.header.id = MessageTypes::ServerAccept;
			client->send(msg);
			return true;
		}

		void TestServerLayer::onMessage(Ref<Connection> client, Message& msg)
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
				Message responseMsg;
				responseMsg.header.id = MessageTypes::ServerMessage;
				responseMsg << client->getID();
				BroadcastToClients(responseMsg, client);
			}
			break;
			}
		}
	}
}