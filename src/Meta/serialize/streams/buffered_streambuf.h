#pragma once

namespace Engine {
namespace Serialize {

    struct BufferedMessageHeader {
        uint64_t mMsgSize;
    };

    struct BufferedSendMessage {
        std::vector<char> mData;
    };

    struct META_EXPORT buffered_streambuf : std::basic_streambuf<char> {
    public:
        buffered_streambuf(std::unique_ptr<std::basic_streambuf<char>> buffer);
        buffered_streambuf(const buffered_streambuf &) = delete;
        buffered_streambuf(buffered_streambuf &&) = delete;

        virtual ~buffered_streambuf();

        void beginMessageWrite();
        void endMessageWrite();

        bool beginMessageRead();
        void endMessageRead();

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

        struct BufferedMessage {
            BufferedMessageHeader mHeader;
            char mData[0];
        };

        std::queue<BufferedSendMessage> mBufferedSendMsgs;

        std::unique_ptr<std::basic_streambuf<char>> mBuffer;
    };
}
}
