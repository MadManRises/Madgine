#include "interfaceslib.h"

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
			mSlaveStream(nullptr),
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
			mSocket(other.mSocket),
			mIsServer(other.mIsServer),
			mSlaveStream(nullptr),
			mConnectionEstablished(this)
		{
			for (std::pair<const Serialize::ParticipantId, NetworkStream>& stream : other.mStreams)
				if (!stream.second.isClosed())
					other.moveConnection(stream.first, this);
			other.mSocket = Invalid_Socket;
			other.mIsServer = false;
			if (other.mSlaveStream)
			{
				mSlaveStream = new NetworkStream(std::move(*other.mSlaveStream), *this);
				setSlaveStream(mSlaveStream, false);
				delete other.mSlaveStream;
				other.mSlaveStream = nullptr;
			}
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

		Serialize::StreamError NetworkManager::connect(const std::string& url, int portNr, int timeout)
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

			mConnectionEstablished.queue_direct(timeout);

			return Serialize::NO_ERROR;
		}

		void NetworkManager::connect_async(const std::string& url, int portNr, int timeout)
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
				assert(mStreams.empty() && mSlaveStream == nullptr);
				mSocket = Invalid_Socket;
			}
		}


		void NetworkManager::acceptConnections()
		{
			if (isConnected())
			{
				if (mIsServer)
				{
					Serialize::StreamError error;
					SocketId sock;
					std::tie(sock, error) = SocketAPI::accept(mSocket);
					while (error != Serialize::TIMEOUT)
					{
						if (sock != Invalid_Socket)
							addMasterStream(NetworkStream(sock, *this, createStreamId()));
						std::tie(sock, error) = SocketAPI::accept(mSocket);
					}
				}
			}
		}

		Serialize::StreamError NetworkManager::acceptConnection(int timeout)
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
						return addMasterStream(NetworkStream(sock, *this, createStreamId()));
					}
					return error;
				}
			}
			return Serialize::NO_SERVER;
		}

		int NetworkManager::clientCount() const
		{
			return mStreams.size();
		}

		bool NetworkManager::isConnected() const
		{
			return mSocket != Invalid_Socket;
		}

		void NetworkManager::moveConnection(Serialize::ParticipantId id, NetworkManager* to)
		{
			auto it = mStreams.find(id);
			if (to->addMasterStream(std::forward<NetworkStream>(it->second), false) != Serialize::NO_ERROR)
				throw 0;
			NetworkStream& stream = to->mStreams.at(id);
			std::list<Serialize::TopLevelSerializableUnitBase*> newTopLevels;
			set_difference(to->getTopLevelUnits().begin(), to->getTopLevelUnits().end(), getTopLevelUnits().begin(),
			               getTopLevelUnits().end(), back_inserter(newTopLevels));
			for (Serialize::TopLevelSerializableUnitBase* newTopLevel : newTopLevels)
			{
				sendState(stream, newTopLevel);
			}
		}

		void NetworkManager::removeSlaveStream()
		{
			if (mSlaveStream)
			{
				SerializeManager::removeSlaveStream();
				delete mSlaveStream;
				mSlaveStream = nullptr;
				mSocket = Invalid_Socket;
			}
		}

		void NetworkManager::removeMasterStream(Serialize::BufferedInOutStream* stream)
		{
			SerializeManager::removeMasterStream(stream);
			mStreams.erase(stream->id());
		}

		Serialize::StreamError NetworkManager::addMasterStream(NetworkStream&& stream, bool sendState)
		{
			//try_emplace TODO
			auto res = mStreams.emplace(std::piecewise_construct, std::forward_as_tuple(stream.id()),
			                            std::forward_as_tuple(std::forward<NetworkStream>(stream), *this));
			if (!res.second)
				throw 0;
			if (SerializeManager::addMasterStream(&res.first->second, sendState))
				return Serialize::NO_ERROR;
			Serialize::StreamError error = res.first->second.error();
			mStreams.erase(res.first);
			return error;
		}

		void NetworkManager::onConnectionEstablished(int timeout)
		{
			mSlaveStream = new NetworkStream(mSocket, *this);
			Serialize::StreamError error = setSlaveStream(mSlaveStream, true, timeout);
			if (error != Serialize::NO_ERROR)
			{
				delete mSlaveStream;
				mSlaveStream = nullptr;
				mSocket = Invalid_Socket;
			}

			mConnectionResult.emit(error);
		}
	}
}
