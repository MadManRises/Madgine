#include "../../metalib.h"
#include "buffered_streambuf.h"

#include "messagelogger.h"

namespace Engine {
namespace Serialize {

    buffered_streambuf::buffered_streambuf(std::unique_ptr<std::basic_streambuf<char>> buffer)
        : mBytesToRead(sizeof(BufferedMessageHeader))
        , mBuffer(std::move(buffer))
    {
#if ENABLE_MESSAGE_LOGGING
        MessageLogger::registerStream(this);
#endif
    }

    buffered_streambuf::~buffered_streambuf()
    {
#if ENABLE_MESSAGE_LOGGING
        MessageLogger::unregisterStream(this);
#endif
    }

    buffered_streambuf::pos_type buffered_streambuf::seekoff(off_type off, std::ios_base::seekdir dir,
        std::ios_base::openmode mode)
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

    buffered_streambuf::pos_type buffered_streambuf::seekpos(pos_type pos, std::ios_base::openmode mode)
    {
        if (0 <= pos && eback() + pos <= egptr()) {
            setg(eback(), eback() + pos, egptr());
            return pos;
        }
        return pos_type(off_type(-1));
    }

    void buffered_streambuf::extend()
    {
        if (mSendBuffer.empty()) {
            mSendBuffer.resize(128);
            setp(mSendBuffer.data(), mSendBuffer.data() + mSendBuffer.size());
            pbump(sizeof(BufferedMessageHeader));
        } else {
            size_t index = pptr() - pbase();
            mSendBuffer.resize(2 * mSendBuffer.capacity());
            setp(mSendBuffer.data(), mSendBuffer.data() + mSendBuffer.size());
            pbump(static_cast<int>(index));
        }
    }

    buffered_streambuf::int_type buffered_streambuf::overflow(int c)
    {
        if (c != EOF) {
            extend();
            *pptr() = c;
            pbump(1);
            return c;
        }
        return traits_type::eof();
    }

    int buffered_streambuf::sync()
    {
        if (sendMessages() < 0)
            return -1;
        if (mBuffer->pubsync() < 0)
            return -1;
        if (receiveMessages() < 0)
            return -1;
        return 0;
    }

    MessageId buffered_streambuf::beginMessageWriteImpl()
    {
        extend();
        BufferedMessageHeader *header = reinterpret_cast<BufferedMessageHeader *>(mSendBuffer.data());
        header->mMessageId = ++mRunningMessageId;
        return header->mMessageId;
    }

    void buffered_streambuf::endMessageWriteImpl()
    {
        assert(!mSendBuffer.empty());
        BufferedMessageHeader *header = reinterpret_cast<BufferedMessageHeader *>(mSendBuffer.data());
        header->mMsgSize = pptr() - pbase() - sizeof(BufferedMessageHeader);
        assert(header->mMsgSize > 0);
        mSendBuffer.resize(pptr() - pbase());
        mSendBuffer.shrink_to_fit();
        mBufferedSendMsgs.emplace(BufferedSendMessage { std::move(mSendBuffer) });
    }

    MessageId buffered_streambuf::beginMessageReadImpl()
    {
        receiveMessages();
        if (!egptr())
            return 0;
        return mReceiveMessageHeader.mMessageId;
    }

    std::streamsize buffered_streambuf::sendMessages()
    {
        while (!mBufferedSendMsgs.empty()) {
            BufferedSendMessage msg = std::move(mBufferedSendMsgs.front());
            mBufferedSendMsgs.pop();
            int num = mBuffer->sputn(msg.mData.data(), msg.mData.size());
            if (num != msg.mData.size()) {
                return -1;
            }
#if ENABLE_MESSAGE_LOGGING
            MessageLogger::log(this, std::move(msg));
#endif
        }
        return 0;
    }

    std::streamsize buffered_streambuf::receiveMessages()
    {
        int readCount = 0;
        if (!mRecBuffer.empty() && mBytesToRead == 0 && !egptr()) {
            mRecBuffer.clear();
            mBytesToRead = sizeof mReceiveMessageHeader;
        }
        if (mRecBuffer.empty()) {
            assert(mBytesToRead > 0);
            std::streamsize avail = mBuffer->in_avail();
            if (avail < 0) {
                return avail;
            } else if (avail > 0) {
                if (avail > mBytesToRead) {
                    avail = mBytesToRead;
                }
                int num = mBuffer->sgetn(reinterpret_cast<char *>(&mReceiveMessageHeader + 1) - mBytesToRead, avail);
                if (num != avail) {
                    return -1;
                }
                readCount += num;
                mBytesToRead -= num;
                if (mBytesToRead == 0) {
                    assert(mReceiveMessageHeader.mMsgSize > 0);
                    mBytesToRead = mReceiveMessageHeader.mMsgSize;
                    mRecBuffer.resize(mBytesToRead);
                }
            }
        }

        if (!mRecBuffer.empty() && mBytesToRead > 0) {
            std::streamsize avail = mBuffer->in_avail();
            if (avail < 0) {
                return avail;
            } else if (avail > 0) {
                if (avail > mBytesToRead) {
                    avail = mBytesToRead;
                }
                int num = mBuffer->sgetn(mRecBuffer.data() + mReceiveMessageHeader.mMsgSize - mBytesToRead, avail);
                if (num != avail) {
                    return -1;
                }
                readCount += num;
                mBytesToRead -= num;
                if (mBytesToRead == 0) {
                    setg(mRecBuffer.data(), mRecBuffer.data(), mRecBuffer.data() + mReceiveMessageHeader.mMsgSize);
#if ENABLE_MESSAGE_LOGGING
                    MessageLogger::log(this, mReceiveMessageHeader, std::move(mRecBuffer));
                    mRecBuffer = { 1 };
#endif
                }
            }
        }
        return readCount;
    }

}
}
