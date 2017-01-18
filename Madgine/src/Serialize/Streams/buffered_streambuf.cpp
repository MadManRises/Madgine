#include "madginelib.h"
#include "buffered_streambuf.h"

#include "Util\UtilMethods.h"

namespace Engine {
namespace Serialize {

	constexpr size_t buffered_streambuf::BUFFER_SIZE;

/*pos_type FileBufferReader::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode)
{
	if (mode & std::ios_base::in) {
		switch (dir) {
		case std::ios_base::beg:
			if (0 <= off && eback() + off <= egptr()) {
				setg(eback(), eback() + off, egptr());
				return pos_type(off);
			}
			break;
		case std::ios_base::cur:
			if (eback() <= gptr() + off && gptr() + off <= egptr()) {
				setg(eback(), gptr() + off, egptr());
				return pos_type(gptr() - eback());
			}
			break;
		case std::ios_base::end:
			if (eback() <= egptr() + off && off <= 0) {
				setg(eback(), egptr() + off, egptr());
				return pos_type(gptr() - eback());
			}
		}
	}

	return pos_type(off_type(-1));
	
}

pos_type FileBufferReader::seekpos(pos_type pos, std::ios_base::openmode mode)
{
	if (0 <= pos && eback() + pos <= egptr()) {
		setg(eback(), eback() + pos, egptr());
		return pos;
	}
	return pos_type(off_type(-1));
}*/


buffered_streambuf::buffered_streambuf() :
	mMsgInBuffer(false),
	mBytesToRead(sizeof(BufferedMessageHeader)),
	mIsClosed(false)
{
	extend();
}

bool buffered_streambuf::isClosed()
{
	return mIsClosed;
}

void buffered_streambuf::close()
{
	mIsClosed = true;
}

void buffered_streambuf::extend()
{
    mSendBuffer.emplace_back();
    setp(mSendBuffer.back().data(), mSendBuffer.back().data() + BUFFER_SIZE);
}

bool buffered_streambuf::isMessageAvailable()
{
	if (mMsgInBuffer && mBytesToRead == 0 && gptr() == eback())
		return true;
	if (!receive()) {
		mIsClosed = true;
		return false;
	}
	return (mMsgInBuffer && mBytesToRead == 0 && gptr() == eback());
}


buffered_streambuf::int_type buffered_streambuf::overflow(int c)
{
    if (c != EOF){
        extend();
        *pptr() = c;
        pbump(1);
        return c;
    }
    return traits_type::eof();
}

int buffered_streambuf::sync()
{
    auto end = mSendBuffer.end();
    --end;
    for (auto it = mSendBuffer.begin(); it != end; ++it){
		send(it->data(), BUFFER_SIZE);
	}
	if (pptr() - pbase() > 0){
		send(pbase(), pptr() - pbase());
	}
    mSendBuffer.clear();
    extend();
    return 0;
}

void buffered_streambuf::beginMessage()
{
	if (!mIsClosed && (mSendBuffer.size() != 1 || pptr() != pbase()))
		throw 0;
}

void buffered_streambuf::sendMessage()
{
	intptr_t sendBufferIndex = pptr() - pbase();
	mSendMessageHeader.mMsgSize = (mSendBuffer.size() - 1) * BUFFER_SIZE + sendBufferIndex;
	if (send(reinterpret_cast<char*>(&mSendMessageHeader), sizeof(mSendMessageHeader)) != sizeof(mSendMessageHeader)) {
		mIsClosed = true;
		return;
	}
	auto it = mSendBuffer.begin();
	while (mSendMessageHeader.mMsgSize > 0) {
		if (it == mSendBuffer.end())
			throw 0;
		size_t count = std::min(mSendMessageHeader.mMsgSize, BUFFER_SIZE);
		int num = send(it->data(), count);
		if (num == 0) {
			mIsClosed = true;
			return;
		}
		if (num == -1) {
			handleError();
			return;
		}
		if (num != count) {
			throw 0;
		}
		mSendMessageHeader.mMsgSize -= count;
		it = mSendBuffer.erase(it);
	}
	if (mSendBuffer.empty()) {
		extend();
	}
	else {
		if (sendBufferIndex != 0 || mSendBuffer.size() != 1)
			throw 0;
	}
}


buffered_streambuf::int_type buffered_streambuf::underflow()
{
	return traits_type::eof();
}

bool buffered_streambuf::receive()
{
	if (mMsgInBuffer && mBytesToRead == 0) {
		if (gptr() != egptr()) {
			std::cout << "Message not fully read!" << std::endl;
		}
		delete[] mRecBuffer;
		mMsgInBuffer = false;
		mBytesToRead = sizeof(mReceiveMessageHeader);
	}
	if (!mMsgInBuffer) {
		assert(mBytesToRead > 0);
		int num = rec(reinterpret_cast<char*>(&mReceiveMessageHeader + 1) - mBytesToRead, mBytesToRead);
		if (num == 0) {
			return false;
		}
		if (num == -1) {
			return handleError();
		}
		mBytesToRead -= num;
		if (mBytesToRead == 0) {
			mMsgInBuffer = true;
			mBytesToRead = mReceiveMessageHeader.mMsgSize;
			mRecBuffer = new char[mBytesToRead];
		}
		else {
			return true;
		}

	}

	if (mMsgInBuffer && mBytesToRead > 0) {
		int num = rec((mRecBuffer + mReceiveMessageHeader.mMsgSize - mBytesToRead), mBytesToRead);
		if (num == 0) {
			return false;
		}
		if (num == -1) {
			return handleError();
		}
		mBytesToRead -= num;
		if (mBytesToRead == 0) {
			setg(mRecBuffer, mRecBuffer, mRecBuffer + mReceiveMessageHeader.mMsgSize);
		}
		return true;
	}

	throw 0;
}

} // namespace Serialize
} // namespace Core

