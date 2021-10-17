#pragma once

#include "serializestreambuf.h"

#include "pendingrequest.h"

#include "streamstate.h"
#include "Generic/stringutil.h"

namespace Engine {
namespace Serialize {

    struct BufferedMessageHeader {
        uint64_t mMsgSize;
    };

    struct META_EXPORT BufferedStreamData : SerializeStreamData {
    public:
        BufferedStreamData(std::unique_ptr<Formatter> format, SyncManager &mgr, ParticipantId id = 0);
        BufferedStreamData(const BufferedStreamData &) = delete;
        BufferedStreamData(BufferedStreamData &&) noexcept = delete;

        virtual ~BufferedStreamData() = default;

        SyncManager *manager();

        //read
        PendingRequest *fetchRequest(TransactionId id);
        void popRequest(TransactionId id);

        //write
        TransactionId createRequest(ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback);


    private:
        TransactionId mRunningTransactionId = 0;
        std::queue<PendingRequest> mPendingRequests;
    };

    struct META_EXPORT buffered_streambuf : std::basic_streambuf<char> {
    public:
        buffered_streambuf(std::unique_ptr<std::basic_streambuf<char>> buffer);
        buffered_streambuf(const buffered_streambuf &) = delete;
        buffered_streambuf(buffered_streambuf &&) noexcept;

        virtual ~buffered_streambuf();

        //write
        void beginMessage();
        void endMessage();

    protected:

        pos_type seekoff(off_type off, std::ios_base::seekdir dir,
            std::ios_base::openmode mode = std::ios_base::in) override;
        pos_type seekpos(pos_type pos,
            std::ios_base::openmode mode = std::ios_base::in) override;

        int_type overflow(int c = EOF) override;
        int_type underflow() override;

        std::streamsize showmanyc() override;

        int sync() override;
        void extend();
        std::streamsize receiveMessages();
        std::streamsize sendMessages();

    private:        

        //read
        uint64_t mBytesToRead;
        BufferedMessageHeader mReceiveMessageHeader;
        std::vector<char> mRecBuffer;

        //write
        std::vector<char> mSendBuffer;

        struct BufferedSendMessage {
            std::vector<char> mData;
        };

        std::list<BufferedSendMessage> mBufferedSendMsgs;

        std::unique_ptr<std::basic_streambuf<char>> mBuffer;
    };
}
}
