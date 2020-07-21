#include "../../moduleslib.h"
#include "buffered_streambuf.h"
#include "../syncmanager.h"
#include "../formatter.h"

namespace Engine {
namespace Serialize {

    buffered_streambuf::buffered_streambuf(std::unique_ptr<Formatter> format, SyncManager &mgr, ParticipantId id)
        : SerializeStreambuf(std::move(format), mgr, id)
        , mIsClosed(false)
        , mBytesToRead(sizeof(BufferedMessageHeader))
    {
        extend();
    }

    buffered_streambuf::buffered_streambuf(buffered_streambuf &&other) noexcept
        : SerializeStreambuf(std::move(other))
        , mIsClosed(other.mIsClosed)
        , mBytesToRead(other.mBytesToRead)
        , mReceiveMessageHeader(other.mReceiveMessageHeader)
        , mRecBuffer(std::forward<std::vector<char>>(other.mRecBuffer))
        , mSendBuffer(std::forward<std::vector<char>>(other.mSendBuffer))
        , mBufferedSendMsgs(std::forward<std::list<BufferedSendMessage>>(other.mBufferedSendMsgs))
    {
        setg(mRecBuffer.data(), mRecBuffer.data() + (other.gptr() - other.eback()), mRecBuffer.data() + mRecBuffer.size());
        setp(mSendBuffer.data(), mSendBuffer.data() + mSendBuffer.size());
        pbump(static_cast<int>(other.pptr() - other.pbase()));
        other.mIsClosed = true;
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

    bool buffered_streambuf::isClosed()
    {
        return mState != StreamState::OK;
    }

    void buffered_streambuf::close(StreamState cause)
    {
        assert(!isClosed());
        assert(cause != StreamState::OK);
        mState = cause;        
    }

    void buffered_streambuf::extend()
    {
        if (mSendBuffer.empty()) {
            mSendBuffer.resize(128);
            setp(mSendBuffer.data(), mSendBuffer.data() + mSendBuffer.size());
        } else {
            size_t index = pptr() - pbase();
            mSendBuffer.resize(2 * mSendBuffer.capacity());
            setp(mSendBuffer.data(), mSendBuffer.data() + mSendBuffer.size());
            pbump(static_cast<int>(index));
        }
    }

    bool buffered_streambuf::isMessageAvailable()
    {
        if (isClosed())
            return false;
        if (!mRecBuffer.empty() && mBytesToRead == 0 && gptr() == eback())
            return true;
        receive();
        return !mRecBuffer.empty() && mBytesToRead == 0 && gptr() == eback();
    }

    PendingRequest *buffered_streambuf::fetchRequest(TransactionId id)
    {
        if (id == 0)
            return nullptr;
        assert(mPendingRequests.front().mId == id);
        return &mPendingRequests.front();
    }

    void buffered_streambuf::popRequest(TransactionId id)
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
        return sendMessages() == 0 ? 0 : -1; // TODO return value
    }

    TransactionId buffered_streambuf::createRequest(ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void *)> callback)
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
        if (mIsClosed)
            return;
        if (pptr() != pbase())
            std::terminate();
    }

    void buffered_streambuf::endMessage()
    {
        if (mIsClosed)
            return;

        BufferedSendMessage &msg = mBufferedSendMsgs.emplace_back();
        msg.mHeaderSent = false;
        msg.mBytesSent = 0;
        msg.mHeader.mMsgSize = pptr() - pbase();
        mSendBuffer.resize(msg.mHeader.mMsgSize);
        mSendBuffer.shrink_to_fit();
        mSendBuffer.swap(msg.mData);

        extend();
    }

    int buffered_streambuf::sendMessages()
    {
        if (mIsClosed)
            return -1;
        for (auto it = mBufferedSendMsgs.begin(); it != mBufferedSendMsgs.end(); it = mBufferedSendMsgs.erase(it)) {
            if (!it->mHeaderSent) {
                int num = send(reinterpret_cast<char *>(&it->mHeader), sizeof it->mHeader);
                if (num == -1) {
                    handleError();
                    return mIsClosed ? -1 : static_cast<int>(mBufferedSendMsgs.size());
                }
                if (num != sizeof it->mHeader) {
                    std::terminate();
                }
                it->mHeaderSent = true;
            }
            while (it->mBytesSent < it->mHeader.mMsgSize) {

                size_t count = it->mHeader.mMsgSize - it->mBytesSent;
                int num = send(it->mData.data() + it->mBytesSent, count);
                if (num == 0) {
                    close(StreamState::SEND_FAILURE);
                    return -1;
                }
                if (num == -1) {
                    handleError();
                    return mIsClosed ? -1 : static_cast<int>(mBufferedSendMsgs.size());
                }
                it->mBytesSent += num;
            }
        }
        return 0;
    }

    StreamState buffered_streambuf::state() const
    {
        return mState;
    }

    buffered_streambuf::int_type buffered_streambuf::underflow()
    {
        return traits_type::eof();
    }

    void buffered_streambuf::receive()
    {
        if (!mRecBuffer.empty() && mBytesToRead == 0) {
            if (gptr() != egptr()) {
                LOG_WARNING(Database::Exceptions::messageNotFullyRead(manager()->name()));
            }
            mRecBuffer.clear();
            mBytesToRead = sizeof mReceiveMessageHeader;
        }
        if (mRecBuffer.empty()) {
            assert(mBytesToRead > 0);
            int num = recv(reinterpret_cast<char *>(&mReceiveMessageHeader + 1) - mBytesToRead, mBytesToRead);
            if (num == 0) {
                close(StreamState::RECEIVE_FAILURE);
                return;
            }
            if (num == -1) {
                handleError();
                return;
            }
            mBytesToRead -= num;
            if (mBytesToRead == 0) {
                mBytesToRead = mReceiveMessageHeader.mMsgSize;
                mRecBuffer.resize(mBytesToRead);
            }
        }

        if (!mRecBuffer.empty() && mBytesToRead > 0) {
            int num = recv(mRecBuffer.data() + mReceiveMessageHeader.mMsgSize - mBytesToRead, mBytesToRead);
            if (num == 0) {
                close(StreamState::RECEIVE_FAILURE);
                return;
            }
            if (num == -1) {
                handleError();
                return;
            }
            mBytesToRead -= num;
            if (mBytesToRead == 0) {
                setg(mRecBuffer.data(), mRecBuffer.data(), mRecBuffer.data() + mReceiveMessageHeader.mMsgSize);
            }
        }
    }

    SyncManager *buffered_streambuf::manager()
    {
        return static_cast<SyncManager*>(SerializeStreambuf::manager());
    }

} 
}
