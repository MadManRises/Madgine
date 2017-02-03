#pragma once

#include "Serialize/Streams/bufferedstream.h"
#include "networkbuffer.h"

namespace Engine {
	namespace Network {



		class INTERFACES_EXPORT NetworkStream : 
			public Serialize::BufferedInOutStream
			 {

		public:
			NetworkStream(UINT_PTR socket, Serialize::SerializeManager &mgr);
			virtual ~NetworkStream();


		private:
			UINT_PTR mSocket; // = SOCKET

			NetworkBuffer mBuffer;			

		};

		

	}
}

