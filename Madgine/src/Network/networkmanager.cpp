#include "madginelib.h"

#include "networkmanager.h"

#include <WinSock2.h>

#include "Serialize\serializemanager.h"

namespace Engine {
	namespace Network {


		NetworkManager::NetworkManager() :			
			mSocket(0)
		{
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

		NetworkManager::~NetworkManager()
		{
			close();
			WSACleanup();
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
			mSerializer.setMaster(true);
			

			return true;
		}

		void NetworkManager::close()
		{
			if (mSocket) {
				closesocket(mSocket);
				mSocket = 0;
			}
		}

		void NetworkManager::update()
		{
			if (mSocket) {
				if (mIsServer){
					fd_set readSet;
					FD_ZERO(&readSet);
					FD_SET(mSocket, &readSet);
					timeval timeout;
					timeout.tv_sec = 0;  // Zero timeout (poll)
					timeout.tv_usec = 0;
					while (select(mSocket, &readSet, NULL, NULL, &timeout) > 0) {
						mStreams.emplace_back(accept(mSocket, NULL, NULL), mSerializer);
						mSerializer.addBroadcastStream(&mStreams.back());
					}

					for (NetworkStream &stream : mStreams) {

						if (stream.isMessageAvailable()) {
							mSerializer.readMessage(stream);
						}
					}
				}
			}
		}

		Serialize::SerializeManager & NetworkManager::manager()
		{
			return mSerializer;
		}

	}
}
