#include "../../metalib.h"
#include "buffered_streambuf.h"
#include "../formatter.h"
#include "../syncmanager.h"

namespace Engine {
namespace Serialize {

    buffered_streambuf::buffered_streambuf(std::unique_ptr<std::basic_streambuf<char>> buffer)
        : mBytesToRead(sizeof(BufferedMessageHeader))
        , mBuffer(std::move(buffer))
    {
    }

    buffered_streambuf::buffered_streambuf(buffered_streambuf &&other) noexcept
        : std::basic_streambuf<char>(std::move(other))
        , mBytesToRead(other.mBytesToRead)
        , mReceiveMessageHeader(other.mReceiveMessageHeader)
        , mRecBuffer(std::forward<std::vector<char>>(other.mRecBuffer))
        , mSendBuffer(std::forward<std::vector<char>>(other.mSendBuffer))
        , mBufferedSendMsgs(std::forward<std::list<BufferedSendMessage>>(other.mBufferedSendMsgs))
    {
        setg(mRecBuffer.data(), mRecBuffer.data() + (other.gptr() - other.eback()), mRecBuffer.data() + mRecBuffer.size());
        setp(mSendBuffer.data(), mSendBuffer.data() + mSendBuffer.size());
        pbump(static_cast<int>(other.pptr() - other.pbase()));
    }

    buffered_streambuf::~buffered_streambuf()
    {
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

    PendingRequest *BufferedStreamData::fetchRequest(TransactionId id)
    {
        if (id == 0)
            return nullptr;
        assert(mPendingRequests.front().mId == id);
        return &mPendingRequests.front();
    }

    void BufferedStreamData::popRequest(TransactionId id)
    {
        assert(id != 0);
        assert(mPendingRequests.front().mId == id);
        mPendingRequests.pop();
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

    TransactionId BufferedStreamData::createRequest(ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void *)> callback)
    {
        if (requesterTransactionId == 0 && !callback)
            return 0;

        assert(callback || requester != 0);
        TransactionId id = ++mRunningTransactionId;
        mPendingRequests.push({ id, requester, requesterTransactionId, std::move(callback) });
        return id;
    }

    void buffered_streambuf::beginMessage()
    {
        assert(pptr() == pbase());
    }

    void buffered_streambuf::endMessage()
    {
        BufferedSendMessage &msg = mBufferedSendMsgs.emplace_back();
        BufferedMessageHeader *header = reinterpret_cast<BufferedMessageHeader*>(mSendBuffer.data());
        header->mMsgSize = pptr() - pbase() - sizeof(BufferedMessageHeader);
        mSendBuffer.resize(pptr() - pbase());
        mSendBuffer.shrink_to_fit();
        mSendBuffer.swap(msg.mData);
        setp(nullptr, nullptr);
    }

    std::streamsize buffered_streambuf::sendMessages()
    {
        for (auto it = mBufferedSendMsgs.begin(); it != mBufferedSendMsgs.end(); it = mBufferedSendMsgs.erase(it)) {
            int num = mBuffer->sputn(it->mData.data(), it->mData.size());
            if (num != it->mData.size()) {
                return -1;
            }
        }
        return 0;
    }

    buffered_streambuf::int_type buffered_streambuf::underflow()
    {
        return traits_type::eof();
    }

    std::streamsize buffered_streambuf::showmanyc()
    {
        if (receiveMessages() < 0)
            return -1;
        return egptr() - gptr();
    }

    std::streamsize buffered_streambuf::receiveMessages()
    {
        int readCount = 0;
        if (!mRecBuffer.empty() && mBytesToRead == 0) {
            if (gptr() != egptr()) {
                throw 0; //LOG_WARNING("Message not fully read! (" << manager()->name() << ")");
            }
            mRecBuffer.clear();
            mBytesToRead = sizeof mReceiveMessageHeader;
        }
        if (mRecBuffer.empty()) {
            assert(mBytesToRead > 0);
            std::streamsize avail = mBuffer->in_avail();
            if (avail < 0) {
                return avail;
            } else {
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
                    mBytesToRead = mReceiveMessageHeader.mMsgSize;
                    mRecBuffer.resize(mBytesToRead);
                }
            }
        }

        if (!mRecBuffer.empty() && mBytesToRead > 0) {
            std::streamsize avail = mBuffer->in_avail();
            if (avail < 0) {
                return avail;
            } else {
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
                }
            }
        }
        return readCount;
    }

    BufferedStreamData::BufferedStreamData(std::unique_ptr<Formatter> format, SyncManager &mgr, ParticipantId id)
        : SerializeStreamData(std::move(format), mgr, id)
    {
    }

    SyncManager *BufferedStreamData::manager()
    {
        return static_cast<SyncManager *>(SerializeStreamData::manager());
    }

}
}
