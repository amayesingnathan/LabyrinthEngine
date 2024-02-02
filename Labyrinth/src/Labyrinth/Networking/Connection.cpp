#include "Lpch.h"
#include "Connection.h"

#include "ServerLayer.h"

namespace Laby {

	Connection::Connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, TSQueue<OwnedMessage>& qIn)
		: mIOContext(asioContext), mSocket(std::move(socket)), mQMessagesIn(qIn)
	{
		mOwnerType = parent;

		if (mOwnerType == Owner::Server)
		{
			mHandshakeOut = u64(std::chrono::system_clock::now().time_since_epoch().count());

			mHandshakeCheck = Scramble(mHandshakeOut);
		}
		else
		{
			mHandshakeOut = 0;
			mHandshakeIn = 0;
		}
	}

	void Connection::ConnectToClient(ServerLayer* server, u32 uid)
	{
		if (mOwnerType != Owner::Server)
			return;

		if (!mSocket.is_open())
			return;

		mID = uid;

		WriteValidation();
		ReadValidation(server);
	}

	void Connection::ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
	{
		if (mOwnerType != Owner::Client) 
			return;

		asio::async_connect(mSocket, endpoints, [this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
		{
			if (!ec)
			ReadValidation();
			else
				LAB_CORE_WARN("[{0}] Failed to connect to server.", mID);
		});
	}

	void Connection::Disconnect()
	{
		if (IsConnected()) 
			asio::post(mIOContext, [this]() { mSocket.close(); });
	}

	void Connection::Send(const Message& msg)
	{
		asio::post(mIOContext, [this, msg]()
		{
			//If the messages out queue is not empty, then messages are already being written...
			bool currentlyWriting = !mQMessagesOut.empty();
			mQMessagesOut.push(msg);

			if (!currentlyWriting) 
				WriteHeader();  // ...so we only want to start writing messages if it isn't already doing so 
		});
	}

	void Connection::WriteHeader()
	{
		asio::async_write(mSocket, asio::buffer(&mQMessagesOut.front().header, sizeof(MessageHeader)),
			[this](std::error_code ec, usize length)
			{
				if (!ec)
				{
					if (mQMessagesOut.front().body.Size() > 0)
						WriteBody();
					else
					{
						mQMessagesOut.pop();

						if (!mQMessagesOut.empty())
							WriteHeader();
					}
				}
				else
				{
					LAB_CORE_WARN("[{0}] Write Header Fail.", mID);
					mSocket.close();
				}
			});
	}

	void Connection::WriteBody()
	{
		asio::async_write(mSocket, asio::buffer(mQMessagesOut.front().body.Data(), mQMessagesOut.front().body.Size()),
			[this](std::error_code ec, usize length)
			{
				if (!ec)
				{
					mQMessagesOut.pop();

					if (!mQMessagesOut.empty())
						WriteHeader();
				}
				else
				{
					LAB_CORE_WARN("[{0}] Write Body Fail.", mID);
					mSocket.close();
				}
			});
	}

	void Connection::ReadHeader()
	{
		asio::async_read(mSocket, asio::buffer(&mMsgTemporaryIn.header, sizeof(MessageHeader)), [this](std::error_code ec, usize length)
		{
			if (!ec)
			{
				if (mMsgTemporaryIn.header.size > 0)
				{
					mMsgTemporaryIn.body.Resize(mMsgTemporaryIn.header.size);
					ReadBody();
				}
				else
					AddToIncomingMessageQueue();
			}
			else
			{
				LAB_CORE_WARN("[{0}] Read Header Fail.", mID);
				mSocket.close();
			}
		});
	}

	void Connection::ReadBody()
	{
		asio::async_read(mSocket, asio::buffer(mMsgTemporaryIn.body.Data(), mMsgTemporaryIn.body.Size()), [this](std::error_code ec, usize length)
		{
			if (!ec)
			AddToIncomingMessageQueue();
			else
			{
				LAB_CORE_WARN("[{0}] Read Body Fail.", mID);
				mSocket.close();
			}
		});
	}

	void Connection::AddToIncomingMessageQueue()
	{
		if (mOwnerType == Owner::Server)
			mQMessagesIn.push({ Ref<Connection>(this), mMsgTemporaryIn });
		else
			mQMessagesIn.push({ nullptr, mMsgTemporaryIn });

		ReadHeader();
	}

	u64 Connection::Scramble(u64 input)
	{
		u64 out = input ^ 0xA86DEBA86581;
		constexpr i32 ver = StaticCast<i32>((f32)LABYRINTH_VER * 0.3142178);
		out = (out & ver) >> 4 | (out & ver) << 4;
		return out ^ 0xA88DABE51581;
	}

	void Connection::WriteValidation()
	{
		asio::async_write(mSocket, asio::buffer(&mHandshakeOut, sizeof(u64)), [this](std::error_code ec, usize length)
		{
			if (!ec)
			{
				if (mOwnerType == Owner::Client)
					ReadHeader();
			}
			else
			{
				LAB_CORE_WARN("[{0}] Write Validation Fail.", mID);
				mSocket.close();
			}
		});
	}

	void Connection::ReadValidation(ServerLayer* server)
	{
		asio::async_read(mSocket, asio::buffer(&mHandshakeIn, sizeof(u64)), [this, server](std::error_code ec, usize length)
		{
			if (ec)
			{
				LAB_CORE_WARN("[{0}] Read Validation Fail.", mID);
				mSocket.close();
				return;
			}

			if (mOwnerType == Owner::Client)
			{
				mHandshakeOut = Scramble(mHandshakeIn);
				WriteValidation();
				return;
			}

			if (mHandshakeIn != mHandshakeCheck)
			{
				LAB_CORE_WARN("Client Disconnected (Failed Validation)");
				mSocket.close();
				return;
			}

			LAB_CORE_INFO("Client Validated");
			server->OnClientValidated(Ref<Connection>(this));

			ReadHeader();
		});
	}
}