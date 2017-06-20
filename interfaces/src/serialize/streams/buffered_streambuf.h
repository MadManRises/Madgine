#pragma once

#include "../serializable.h"

namespace Engine {
namespace Serialize {




	struct BufferedMessageHeader {
		size_t mMsgSize;
	};


class INTERFACES_EXPORT buffered_streambuf : public std::basic_streambuf<char> {

public:
	buffered_streambuf();
	buffered_streambuf(const buffered_streambuf &) = delete;
	buffered_streambuf(buffered_streambuf &&);

	virtual ~buffered_streambuf();

	virtual bool isClosed();
	virtual void close(StreamError cause = NO_ERROR);

	//read
	bool isMessageAvailable();

	//write
	void beginMessage();
	void endMessage();

	void sendMessages();

	StreamError closeCause();

protected:
	virtual StreamError getError() = 0;
	void handleError();
	

	virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir,
		std::ios_base::openmode mode = std::ios_base::in) override;
	virtual pos_type seekpos(pos_type pos,
		std::ios_base::openmode mode = std::ios_base::in) override;

	virtual size_t rec(char *, size_t) = 0;
	virtual size_t send(char *, size_t) = 0;

	

	virtual int_type overflow(int c = EOF) override;
	virtual int_type underflow() override;
	
	virtual int sync() override;
	void extend();	
	void receive();

private:

	bool mIsClosed;
	StreamError mCloseCause;
	
	//read
	size_t mBytesToRead;
	BufferedMessageHeader mReceiveMessageHeader;
	char *mRecBuffer;
	bool mMsgInBuffer;

	//write
    static constexpr size_t BUFFER_SIZE = 100;

	std::list<std::array<char, BUFFER_SIZE>> mSendBuffer;
	std::list<std::array<char, BUFFER_SIZE>> mStagedSendBuffer;

	struct BufferedSendMessage {
		bool mHeaderSent;
		BufferedMessageHeader mHeader;
	};
	std::list<BufferedSendMessage> mBufferedSendMsgs;
};




} // namespace Serialize
} // namespace Core

