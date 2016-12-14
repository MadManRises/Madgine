#pragma once

namespace Engine {
	namespace Network {
		struct NetworkMessageHeader;
	}
	namespace Serialize {
		class SerializeInStream;
		enum MessageType;
	}
}


namespace Maditor {
	namespace Model {
		namespace Network {

			class MadgineNetworkClient : public QObject{
				Q_OBJECT

			public:
				MadgineNetworkClient();

			public slots:
				void connectToHost();
				void read();
				void connected();

			signals:
				void error();
				void receivedValue(const QVariant &val);

			protected:
				void handleMessage(char *data, size_t size);

				void handleSceneManagerMessage(Engine::Serialize::SerializeInStream &in, Engine::Serialize::MessageType type);

			private:
				QTcpSocket *mSocket;
				QByteArray mBuffer;
			};

		}
	}
}