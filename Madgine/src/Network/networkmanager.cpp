#include "madginelib.h"

#include "networkmanager.h"

#include <WinSock2.h>

#include "Serialize\serializemanager.h"

namespace Engine {
	namespace Network {

		int NetworkManager::sManagerCount = 0;

		NetworkManager::NetworkManager() :			
			mSocket(0)
		{

			if (sManagerCount == 0) {
				WSADATA w;

				int error = WSAStartup(WINSOCK_VERSION, &w);

				if (error) {
					throw 0;
				}

				if (w.wVersion != WINSOCK_VERSION)
				{
					WSACleanup();
					throw 0;
				}

			}
			++sManagerCount;

		}

		NetworkManager::~NetworkManager()
		{
			close();
			--sManagerCount;
			if (sManagerCount == 0) {
				WSACleanup();
			}
		}

		bool NetworkManager::startServer(int port)
		{
			

			SOCKADDR_IN addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);

			addr.sin_addr.s_addr = htonl(INADDR_ANY);

			mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (mSocket == INVALID_SOCKET) {
				return false;
			}

			if (bind(mSocket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
			{
				close();
				return false;
			}

			int result = listen(mSocket, SOMAXCONN);		
			if (result == SOCKET_ERROR) {
				close();
				return false;
			}

			mIsServer = true;
			setMaster(true);
			
			return true;
		}

		bool NetworkManager::connect(const std::string & url, int portNr, int timeout)
		{

			//Fill out the information needed to initialize a socket…
			SOCKADDR_IN target; //Socket address information

			target.sin_family = AF_INET; // address family Internet
			target.sin_port = htons(portNr); //Port to connect on
			target.sin_addr.s_addr = inet_addr(url.c_str()); //Target IP

			mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
			if (mSocket == INVALID_SOCKET)
			{
				return false; //Couldn't create the socket
			}

			if (setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR) {
				close();
				return false;
			}

			//Try connecting...

			if (::connect(mSocket, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
			{
				close();
				return false; //Couldn't connect
			}

			mIsServer = false;
			setMaster(false);

			mStreams.emplace_back(mSocket, *this);

			return true;
		}

		void NetworkManager::close()
		{
			if (mSocket) {

				if (mIsServer) {
					closesocket(mSocket);
				}
				mStreams.clear();
				
				mSocket = 0;
			}
		}

		void NetworkManager::receiveMessages()
		{
			for (auto it = mStreams.begin(); it != mStreams.end();) {
				if (it->isValid()) {

					if (it->isMessageAvailable()) {
						readMessage(*it);
					}
					++it;
				}
				else {
					removeBroadcastStream(&*it);
					it = mStreams.erase(it);
				}
			}
		}

		void NetworkManager::acceptConnections()
		{
			if (mSocket) {
				if (mIsServer) {
					fd_set readSet;
					FD_ZERO(&readSet);
					FD_SET(mSocket, &readSet);
					timeval timeout;
					timeout.tv_sec = 0;  // Zero timeout (poll)
					timeout.tv_usec = 0;
					while (select(mSocket, &readSet, NULL, NULL, &timeout) > 0) {
						mStreams.emplace_back(accept(mSocket, NULL, NULL), *this);
						addBroadcastStream(&mStreams.back());
					}
				}
			}
		}

		int NetworkManager::clientCount()
		{
			return mStreams.size();
		}

		std::list<Serialize::SerializeOutStream*> NetworkManager::getMessageTargets(Serialize::SerializableUnit *unit)
		{
			std::list<Serialize::SerializeOutStream*> result = SerializeManager::getMessageTargets(unit);
			for (Serialize::SerializeOutStream *out : result) {
				out->beginMessage();
			}
			return result;
		}

	}
}
