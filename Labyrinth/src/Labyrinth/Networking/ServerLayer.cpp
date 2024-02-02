#include "Lpch.h"
#include "ServerLayer.h"

namespace Laby {

	ServerLayer::ServerLayer(uint16_t port)
		: mAsioAcceptor(mIOContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{
	}

	bool ServerLayer::Start()
	{
		try
		{
			WaitForClientConnection();

			mIOContextThread = std::thread([this]() { mIOContext.run(); });
		}
		catch (std::exception& e)
		{
			LAB_CORE_ERROR("Exception: {0}", e.what());
			return false;
		}

		LAB_CORE_TRACE("Server Started!");
		return true;

	}

	void ServerLayer::Stop()
	{
		mIOContext.stop();

		if (mIOContextThread.joinable()) mIOContextThread.join();

		LAB_CORE_TRACE("Server Stopped!");
	}

	void ServerLayer::Update(usize maxMessages)
	{
		usize messageCount = 0;
		while (messageCount < maxMessages && !mQMessagesIn.empty())
		{
			auto msg = mQMessagesIn.pop();
			//EventManager::Post<NetMessageEvent>(msg.remote->getID(), std::move(msg.msg));
			messageCount++;
		}
	}

	void ServerLayer::WaitForClientConnection()
	{
		mAsioAcceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket)
		{
			if (!ec)
			{
				LAB_CORE_TRACE("New Connection {0}", socket.remote_endpoint());

				Ref<Connection> newConn = Ref<Connection>::Create(Connection::Owner::Server, mIOContext, std::move(socket), mQMessagesIn);

				if (OnClientConnect(newConn))
				{
					mConnections.push_back(std::move(newConn));
					mConnections.back()->ConnectToClient(this, mIDCounter++);

					LAB_CORE_TRACE("[{0}] Connection Approved", mConnections.back()->GetID());
				}
				else
				{
					LAB_CORE_INFO("[-----] Connection Denied");
				}
			}
			else
				LAB_CORE_ERROR("New Connection Error {0}", ec.message());

			WaitForClientConnection();
		});
	}

	void ServerLayer::MessageClient(UUID clientID, const Message& msg)
	{
		auto it = std::find_if(mConnections.begin(), mConnections.end(),
			[clientID](const Ref<Connection>& client) { return clientID == client->GetID(); });

		if (it == mConnections.end())
		{
			LAB_CORE_ERROR("Could not find client to message!");
			return;
		}

		Ref<Connection> client = *it;

		if (!client || !client->IsConnected())
		{
			LAB_CORE_ERROR("Client not connected! Disconnecting...");
			OnClientDisconnect(client);
			client.Reset();
			std::erase(mConnections, client);
			return;
		}

		(*it)->Send(msg);
	}

	void ServerLayer::BroadcastToClients(const Message& msg, UUID ignoreClient)
	{
		for (auto& client : mConnections)
			MessageEachClient(client, msg, ignoreClient);

		std::erase(mConnections, nullptr);
	}

	void ServerLayer::MessageEachClient(Ref<Connection> client, const Message& msg, UUID ignoreClient)
	{
		if (!client || !client->IsConnected())
		{
			OnClientDisconnect(client);
			client.Reset();
			return;
		}

		if (client->GetID() == ignoreClient)
			return;

		client->Send(msg);
	}
}