#include "../interfaceslib.h"

#include "networkmanager.h"


namespace Engine
{
	namespace Network
	{
		int NetworkManager::sManagerCount = 0;

		NetworkManager::NetworkManager(const std::string& name) :
			SerializeManager(name),
			mSocket(Invalid_Socket),
			mIsServer(false),
			mConnectionEstablished(this)
		{
			if (sManagerCount == 0)
			{
				if (!SocketAPI::init())
					throw 0;
			}
			++sManagerCount;
		}

		NetworkManager::NetworkManager(NetworkManager&& other) noexcept :
			SerializeManager(std::forward<NetworkManager>(other)),
			mSocket(std::exchange(other.mSocket, Invalid_Socket)),
			mIsServer(std::exchange(other.mIsServer, false)),
			mConnectionEstablished(this)
		{
			++sManagerCount;
		}

		NetworkManager::~NetworkManager()
		{
			close();
			--sManagerCount;
			if (sManagerCount == 0)
			{
				SocketAPI::finalize();
			}
		}

		bool NetworkManager::startServer(int port)
		{
			if (isConnected())
				return false;

			mSocket = SocketAPI::socket(port);

			if (isConnected())
			{
				mIsServer = true;
				return true;
			}
			return false;
		}

		Serialize::StreamError NetworkManager::connect(const std::string& url, int portNr, TimeOut timeout)
		{
			if (isConnected())
			{
				mConnectionResult.emit(Serialize::ALREADY_CONNECTED);
				return Serialize::ALREADY_CONNECTED;
			}

			Serialize::StreamError error;

			std::tie(mSocket, error) = SocketAPI::connect(url, portNr);

			if (!isConnected())
			{
				mConnectionResult.emit(error);
				return error;
			}

			mConnectionEstablished.queue(timeout);

			return Serialize::NO_ERROR;
		}

		void NetworkManager::connect_async(const std::string& url, int portNr, TimeOut timeout)
		{
			std::thread(&NetworkManager::connect, this, url, portNr, timeout).detach();
		}

		void NetworkManager::close()
		{
			if (isConnected())
			{
				removeAllStreams();
				if (mIsServer)
				{
					SocketAPI::closeSocket(mSocket);
					mIsServer = false;
				}
				mSocket = Invalid_Socket;
			}
		}


		int NetworkManager::acceptConnections(int limit)
		{
			int count = 0;
			if (isConnected())
			{
				if (mIsServer)
				{
					Serialize::StreamError error;
					SocketId sock;
					std::tie(sock, error) = SocketAPI::accept(mSocket);
					while (error != Serialize::TIMEOUT && (limit == -1 || count < limit))
					{
						if (sock != Invalid_Socket)
						{
							if (addMasterStream(Serialize::BufferedInOutStream{ std::make_unique<NetworkBuffer>(sock, *this, createStreamId()) })) {
								++count;								
							}
						}
						std::tie(sock, error) = SocketAPI::accept(mSocket);
					}
				}
			}
			return count;
		}

		Serialize::StreamError NetworkManager::acceptConnection(TimeOut timeout)
		{
			if (isConnected())
			{
				if (mIsServer)
				{
					Serialize::StreamError error;
					SocketId sock;
					std::tie(sock, error) = SocketAPI::accept(mSocket, timeout);
					if (sock != Invalid_Socket)
					{
						Serialize::BufferedInOutStream stream{ std::make_unique<NetworkBuffer>(sock, *this, createStreamId()) };
						if (addMasterStream(std::move(stream)))
						{
							return Serialize::NO_ERROR;
						}
						error = stream.error();
					}
					return error;
				}
			}
			return Serialize::NO_SERVER;
		}


		bool NetworkManager::isConnected() const
		{
			return mSocket != Invalid_Socket;
        }

        bool NetworkManager::moveMasterStream(
            Serialize::ParticipantId streamId, NetworkManager *target) {
            return SerializeManager::moveMasterStream(streamId, target);
        }

		SignalSlot::SignalStub<Serialize::StreamError>& NetworkManager::connectionResult()
		{
			return mConnectionResult;
		}

		void NetworkManager::onConnectionEstablished(TimeOut timeout)
		{
			bool success = setSlaveStream(Serialize::BufferedInOutStream{ std::make_unique<NetworkBuffer>(mSocket, *this) }, true, timeout);

			mConnectionResult.emit(success ? Serialize::NO_ERROR : Serialize::UNKNOWN_ERROR);
		}
	}
}
