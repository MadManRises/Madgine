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

	virtual bool isClosed();


protected:
	virtual bool handleError() = 0;

	void close();

private:
	bool mIsClosed;


public:

	//read
	bool isMessageAvailable();

protected:
	/*virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir,
		std::ios_base::openmode mode = std::ios_base::in) override;
	virtual pos_type seekpos(pos_type pos,
		std::ios_base::openmode mode = std::ios_base::in) override;*/

	virtual size_t rec(char *, size_t) = 0;

	virtual int_type underflow() override;
	bool receive();

private:

	size_t mBytesToRead;
	BufferedMessageHeader mReceiveMessageHeader;
	char *mRecBuffer;
	bool mMsgInBuffer;

public:
	//write

	void beginMessage();
	void sendMessage();

protected:

	virtual size_t send(char *, size_t) = 0;

    void extend();

    virtual int_type overflow(int c = EOF) override;
    virtual int sync() override;

private:
    static constexpr size_t BUFFER_SIZE = 100;

    std::list<std::array<char, BUFFER_SIZE>> mSendBuffer;

	BufferedMessageHeader mSendMessageHeader;


};




} // namespace Serialize
} // namespace Core

