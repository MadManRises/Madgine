#pragma once

#include "Serialize/Streams/serializestream.h"
#include "networkbuffer.h"

namespace Engine {
	namespace Network {



		class MADGINE_EXPORT NetworkStream : 
			public Serialize::SerializeInStream,
			public Serialize::SerializeOutStream
			 {

		public:
			NetworkStream(UINT_PTR socket, Serialize::SerializeManager &mgr);
			virtual ~NetworkStream();

			bool isMessageAvailable();

			bool isValid();

			virtual void beginMessage() override;
			virtual void endMessage() override;


		private:
			UINT_PTR mSocket; // = SOCKET

			NetworkBuffer mBuffer;
			std::iostream mStream;
			

			

		};

		

	}
}

