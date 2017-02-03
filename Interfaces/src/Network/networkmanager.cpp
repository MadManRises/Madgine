#include "interfaceslib.h"

#include "networkmanager.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "Serialize\serializemanager.h"

namespace Engine {
	namespace Network {

		int NetworkManager::sManagerCount = 0;

		NetworkManager::NetworkManager() :			
			mSocket(0),
			mIsServer(false),
			mSlaveStream(0)
		{

			if (sManagerCount == 0) {
				WSADATA w;

				int error = WSAStartup(MAKEWORD(2, 2), &w);

				if (error) {
					throw 0;
				}

				if (w.wVersion != MAKEWORD(2, 2))
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
			if (mSocket)
				return false;

			mIsServer = true;

			SOCKADDR_IN addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);

			addr.sin_addr.s_addr = htonl(INADDR_ANY);

			mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (mSocket == INVALID_SOCKET) {
				mSocket = 0;
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
						
			return true;
		}

		bool NetworkManager::connect(const std::string & url, int portNr, int timeout)
		{

			if (mSocket)
				return false;

			//Fill out the information needed to initialize a socket…
			SOCKADDR_IN target; //Socket address information

			target.sin_family = AF_INET; // address family Internet
			target.sin_port = htons(portNr); //Port to connect on
			target.sin_addr.s_addr = inet_addr(url.c_str());
			
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


			mSlaveStream = new NetworkStream(mSocket, *this);
			if (!setSlaveStream(mSlaveStream)) {
				delete mSlaveStream;
				mSlaveStream = 0;
				return false;
			}


			return true;
		}

		void NetworkManager::close()
		{
			if (mSocket) {
				clearAllStreams();
				if (mIsServer) {
					closesocket(mSocket);
					mIsServer = false;
				}
				assert(mStreams.empty() && mSlaveStream == 0);
				mSocket = 0;
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
						addMasterStream(&mStreams.back());
					}
				}
			}
		}

		bool NetworkManager::acceptConnection(int timeout) {
			if (mSocket) {
				if (mIsServer) {
					fd_set readSet;
					FD_ZERO(&readSet);
					FD_SET(mSocket, &readSet);
					timeval timeout_s;
					timeout_s.tv_sec = timeout / 1000;
					timeout_s.tv_usec = (timeout % 1000) * 1000;
					if (select(mSocket, &readSet, NULL, NULL, &timeout_s) > 0) {
						mStreams.emplace_back(accept(mSocket, NULL, NULL), *this);
						addMasterStream(&mStreams.back());
						return true;
					}
				}
			}
			return false;
		}

		int NetworkManager::clientCount()
		{
			return mStreams.size();
		}

		void NetworkManager::onSlaveStreamRemoved(Serialize::BufferedInOutStream * stream)
		{
			SerializeManager::onSlaveStreamRemoved(stream);
			if (stream != mSlaveStream)
				throw 0;
			delete mSlaveStream;
			mSlaveStream = 0;
		}

		void NetworkManager::onMasterStreamRemoved(Serialize::BufferedInOutStream * stream)
		{
			SerializeManager::onMasterStreamRemoved(stream);
			mStreams.remove_if([=](NetworkStream &s) {return &s == stream; });
		}

	}
}
