#pragma once

#include "serializestreambuf.h"

#include "pendingrequest.h"

#include "Interfaces/streams/streamresult.h"

namespace Engine {
namespace Serialize {


    struct BufferedMessageHeader {
        size_t mMsgSize;
    };

    struct MODULES_EXPORT buffered_streambuf : SerializeStreambuf {
    public:
        buffered_streambuf(std::unique_ptr<Formatter> format, SyncManager &mgr, ParticipantId id);
        buffered_streambuf(const buffered_streambuf &) = delete;
        buffered_streambuf(buffered_streambuf &&) noexcept;

        virtual ~buffered_streambuf();

        SyncManager *manager();

        virtual bool isClosed();
        virtual void close(StreamResult cause = StreamResult::SUCCESS);

        //read
        bool isMessageAvailable();

        PendingRequest *fetchRequest(TransactionId id);
        void popRequest(TransactionId id);

        //write
        TransactionId createRequest(ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void*)> callback);

        void beginMessage();
        void endMessage();

        int sendMessages();

        StreamResult state() const;

    protected:
        virtual StreamResult getError() = 0;
        void handleError();

        pos_type seekoff(off_type off, std::ios_base::seekdir dir,
            std::ios_base::openmode mode = std::ios_base::in) override;
        pos_type seekpos(pos_type pos,
            std::ios_base::openmode mode = std::ios_base::in) override;

        virtual int recv(char *, size_t) = 0;
        virtual int send(char *, size_t) = 0;

        int_type overflow(int c = EOF) override;
        int_type underflow() override;

        int sync() override;
        void extend();
        void receive();

    private:
        bool mIsClosed;
        StreamResult mState = StreamResult::SUCCESS;

        //read
        size_t mBytesToRead;
        BufferedMessageHeader mReceiveMessageHeader;
        std::vector<char> mRecBuffer;

        //write
        std::vector<char> mSendBuffer;

        struct BufferedSendMessage {
            bool mHeaderSent;
            BufferedMessageHeader mHeader;
            size_t mBytesSent;
            std::vector<char> mData;
        };

        std::list<BufferedSendMessage> mBufferedSendMsgs;

        TransactionId mRunningTransactionId = 0;
        std::queue<PendingRequest> mPendingRequests;
    };
}
}
