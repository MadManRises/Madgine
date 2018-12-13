#pragma once

namespace Engine
{
	namespace Serialize
	{
		struct BufferedMessageHeader
		{
			size_t mMsgSize;
		};


		class INTERFACES_EXPORT buffered_streambuf : public std::basic_streambuf<char>
		{
		public:
			buffered_streambuf();
			buffered_streambuf(const buffered_streambuf&) = delete;
			buffered_streambuf(buffered_streambuf&&) noexcept;

			virtual ~buffered_streambuf();

			virtual bool isClosed();
			virtual void close(StreamError cause = NO_ERROR);

			//read
			bool isMessageAvailable(const std::string& context);

			//write
			void beginMessage();
			void endMessage();

			int sendMessages();

			StreamError closeCause() const;

		protected:
			virtual StreamError getError() = 0;
			void handleError();


			pos_type seekoff(off_type off, std::ios_base::seekdir dir,
			                 std::ios_base::openmode mode = std::ios_base::in) override;
			pos_type seekpos(pos_type pos,
			                 std::ios_base::openmode mode = std::ios_base::in) override;

			virtual int rec(char*, size_t) = 0;
			virtual int send(char*, size_t) = 0;


			int_type overflow(int c = EOF) override;
			int_type underflow() override;

			int sync() override;
			void extend();
			void receive(const std::string& context);

		private:

			bool mIsClosed;
			StreamError mCloseCause;

			//read
			size_t mBytesToRead;
			BufferedMessageHeader mReceiveMessageHeader;
			std::vector<char> mRecBuffer;
			

			//write
			std::vector<char> mSendBuffer;

			struct BufferedSendMessage
			{
				bool mHeaderSent;
				BufferedMessageHeader mHeader;
				size_t mBytesSent;
				std::vector<char> mData;
			};

			std::list<BufferedSendMessage> mBufferedSendMsgs;
		};
	} // namespace Serialize
} // namespace Core