#pragma once

#include "Util\MadgineObject.h"
#include "Serialize\serializemanager.h"
#include "networkstream.h"

namespace Engine {
	namespace Network {

		class MADGINE_EXPORT NetworkManager : 
			public Ogre::Singleton<NetworkManager>,
			public Util::MadgineObject<NetworkManager>,
			public Ogre::GeneralAllocatedObject {

		public:
			NetworkManager();
			virtual ~NetworkManager();

			bool startServer(int portNr);

			void close();

			void update();

			Serialize::SerializeManager &manager();

		private:
			UINT_PTR mSocket; // = SOCKET

			bool mIsServer;
			//std::string mURL;
			int mPort;

			std::list<NetworkStream> mStreams;

			Serialize::SerializeManager mSerializer;

			static constexpr UINT sMessageSignature = 1048;

		};

	}
}

