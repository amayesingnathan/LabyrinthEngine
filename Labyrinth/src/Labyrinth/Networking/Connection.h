#pragma once

#include <Labyrinth/Core/System/Ref.h>
#include <Labyrinth/Core/UUID.h>
#include <Labyrinth/Containers/TSQueue.h>

#include "NetCommon.h"
#include "Message.h"

namespace Laby {

	class ServerLayer;
	struct OwnedMessage;

	class Connection : public RefCounted
	{
	public:
		enum class Owner
		{
			Server,
			Client
		};

	public:
		Connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, TSQueue<OwnedMessage>& qIn);
		virtual ~Connection() {}

		UUID getID() const { return mID; }

	public:
		void connectToClient(ServerLayer* server, u32 uid = 0);
		void connectToServer(const asio::ip::tcp::resolver::results_type& endpoints);
		void disconnect();
		bool isConnected() const { return mSocket.is_open(); }

	public:
		void send(const Message& msg); // ASYNC

	private:
		void WriteHeader(); // ASYNC
		void WriteBody(); // ASYNC
		void ReadHeader(); // ASYNC
		void ReadBody(); // ASYNC

		void AddToIncomingMessageQueue(); // ASYNC

		void WriteValidation(); // ASYNC
		void ReadValidation(ServerLayer* server = nullptr); // ASYNC

		u64 Scramble(u64 input);

	private:
		Owner mOwnerType = Owner::Server;
		UUID mID;

		asio::ip::tcp::socket mSocket;
		asio::io_context& mIOContext;

		TSQueue<Message> mQMessagesOut;
		TSQueue<OwnedMessage>& mQMessagesIn;

		Message mMsgTemporaryIn;

		u64 mHandshakeIn = 0;
		u64 mHandshakeOut = 0;
		u64 mHandshakeCheck = 0;
	};
}