#include "TestServer.h"

namespace Labyrinth
{
	namespace Net {

		void TestServerLayer::onUpdate(Timestep ts)
		{
			update(-1, true);
		}

		bool TestServerLayer::onClientConnect(Ref<Connection<MessageType>> client)
		{
			Message<MessageType> msg;
			msg.header.id = MessageTypes::ServerAccept;
			client->send(msg);
			return true;
		}

		void TestServerLayer::onMessage(Ref<Connection<MessageType>> client, Message<MessageType>& msg)
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
				Message<MessageType> responseMsg;
				responseMsg.header.id = MessageTypes::ServerMessage;
				responseMsg << client->getID();
				BroadcastToClients(responseMsg, client);
			}
			break;
			}
		}
	}
}