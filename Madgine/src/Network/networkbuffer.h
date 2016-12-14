#pragma once

namespace Engine {
	namespace Network {

		struct NetworkMessageHeader {
			size_t mMsgSize;
		};

		class MADGINE_EXPORT NetworkBuffer : 
			public std::basic_streambuf<char>
			 {

		public:
			NetworkBuffer(UINT_PTR socket);
			virtual ~NetworkBuffer();

			bool isMessageAvailable();


			void beginMessage();
			void sendMessage();

			bool isClosed();

		protected:
			virtual int_type underflow() override;
			void extend();
			virtual int_type overflow(int c) override;
			
		private:
			NetworkMessageHeader mReceiveMessageHeader;
			NetworkMessageHeader mSendMessageHeader;

			static constexpr size_t BUFFER_SIZE = 100;
			std::list<std::array<char, BUFFER_SIZE>> mSendBuffer;

			char *mMsgBuffer;
			bool mMsgInBuffer;
			size_t mBytesToRead;

			UINT_PTR mSocket; // = SOCKET

			bool mClosed;
			
		};

	}
}

