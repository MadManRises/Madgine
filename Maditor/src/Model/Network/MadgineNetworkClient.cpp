#include "maditorlib.h"

#include "madgineinclude.h"

#include "MadgineNetworkClient.h"

#include "Serialize\Streams\filebuffer.h"
#include "Serialize\serializablebase.h"

namespace Maditor {
	namespace Model {
		namespace Network {

			MadgineNetworkClient::MadgineNetworkClient() :
				mSocket(new QTcpSocket(this))
			{

				connect(mSocket, &QTcpSocket::readyRead, this, &MadgineNetworkClient::read);
			}

			void MadgineNetworkClient::connectToHost() {
				mSocket->connectToHost("127.0.0.1", 1000);
			}

			


			void MadgineNetworkClient::read()
			{
				mBuffer += mSocket->readAll();
				bool loop = true;
				while (loop && mBuffer.size() >= sizeof(Engine::Network::NetworkMessageHeader)) {
					Engine::Network::NetworkMessageHeader &header = *reinterpret_cast<Engine::Network::NetworkMessageHeader*>(mBuffer.data());
					size_t size = header.mMsgSize;
					if (mBuffer.size() >= sizeof(Engine::Network::NetworkMessageHeader) + size) {
						handleMessage(mBuffer.data() + sizeof(Engine::Network::NetworkMessageHeader), size);
						mBuffer = mBuffer.mid(size + sizeof(Engine::Network::NetworkMessageHeader));
					}
					else {
						loop = false;
					}
				}
			}

			void MadgineNetworkClient::handleMessage(char * data, size_t size)
			{
				Engine::Serialize::MessageHeader &header = *reinterpret_cast<Engine::Serialize::MessageHeader*>(data);

				Engine::Serialize::FileBuffer buffer(data + sizeof(Engine::Serialize::MessageHeader), data + size);
				Engine::Serialize::FileBufferReader reader(buffer);
				std::istream is(&reader);
				Engine::Serialize::SerializeManager mgr;
				Engine::Serialize::SerializeInStream in(is, mgr);
				if (!header.mIsMadgineComponent)
					return;
				switch (header.mMadgineComponent) {
				case Engine::Serialize::TopLevelMadgineObject::SCENE_MANAGER:
					handleSceneManagerMessage(in, header.mType);
					break;
				default:
					emit receivedValue(QString("Got message for unknown Component: %1!").arg((int)header.mMadgineComponent));
				}
				
			}

			void MadgineNetworkClient::handleSceneManagerMessage(Engine::Serialize::SerializeInStream &in, Engine::Serialize::MessageType type)
			{
				switch (type) {
				case Engine::Serialize::ACTION:				

					int index;
					in >> index;

					switch (index) {
					case 0:
						//Entities-List
						Engine::Serialize::Operations op;
						in >> (int&)op;
						switch (op) {
						case Engine::Serialize::ADD_ITEM:
							emit receivedValue(QString("Added new Entity!"));
							break;
						default:
							emit receivedValue(QString("Got unknown message-type for Entity-List: %1!").arg(index));
						}

						break;
					default:
						emit receivedValue(QString("Got SceneManager message for unknown index: %1!").arg(index));
					}
					break;
				default:
					emit receivedValue(QString("Got SceneManager state"));
				}				
			}

			

		}
	}
}