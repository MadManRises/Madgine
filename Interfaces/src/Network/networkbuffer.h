#pragma once

#include "Serialize\Streams\buffered_streambuf.h"


namespace Engine {
	namespace Network {


		class INTERFACES_EXPORT NetworkBuffer : 
			public Serialize::buffered_streambuf
		{

		public:
			NetworkBuffer(UINT_PTR socket);
			virtual ~NetworkBuffer();

		protected:
			// Geerbt über buffered_streambuf
			virtual bool handleError() override;

			virtual size_t rec(char *, size_t) override;

			virtual size_t send(char *, size_t) override;

		private:
			UINT_PTR mSocket; // = SOCKET
			



		};

	}
}

