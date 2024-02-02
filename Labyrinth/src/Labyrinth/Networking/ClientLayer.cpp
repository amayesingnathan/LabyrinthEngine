#include "Lpch.h"
#include "ClientLayer.h"

#include "OwnedMessage.h"

namespace Laby {

	ClientLayer::~ClientLayer()
	{
		if (IsConnected())
			Disconnect();
	}

	void ClientLayer::Send(const Message& msg)
	{
		if (IsConnected())
			mConnection->Send(msg);
	}

	void ClientLayer::Connect(const std::string& host, const u16 port)
	{
		try
		{
			asio::ip::tcp::resolver resolver(mIOContext);
			asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

			mConnection = MakeSingle<Connection>(Connection::Owner::Client, mIOContext, asio::ip::tcp::socket(mIOContext), mQMessagesIn);

			mConnection->ConnectToServer(endpoints);

			mIOContextThread = std::thread([this]() { mIOContext.run(); });
		}
		catch (std::exception& e)
		{
			LAB_CORE_ERROR("Client Exception: {0}", e.what());
			return;
		}

		return;
	}

	void ClientLayer::Disconnect()
	{
		if (IsConnected())
			mConnection->Disconnect();

		mIOContext.stop();

		if (mIOContextThread.joinable())
			mIOContextThread.join();

		mConnection.release();
	}

	void ClientLayer::Update(usize maxMessages)
	{
		if (!IsConnected())
			return;

		usize messageCount = 0;
		while (messageCount < maxMessages && !mQMessagesIn.empty())
		{
			auto msg = mQMessagesIn.pop();
			//EventManager::Post<NetMessageEvent>(msg.remote->getID(), std::move(msg.msg));
			messageCount++;
		}
	}

	bool ClientLayer::IsConnected() const
	{
		if (mConnection)
			return mConnection->IsConnected();

		return false;
	}
}