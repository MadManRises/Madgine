#include "../steamserviceslib.h"

#include "steamstreambuf.h"

namespace Engine {
namespace FirstParty {

    SteamStreambuf::SteamStreambuf(HSteamNetConnection con)
        : mConnection(con)
    {
    }

    SteamStreambuf::~SteamStreambuf()
    {
        SteamNetworkingSockets()->CloseConnection(mConnection, 0, nullptr, true);
    }

    void SteamStreambuf::beginMessageWriteImpl()
    {
        assert(mSendBufferSize == 0);
        extend();
    }

    Serialize::MessageId SteamStreambuf::endMessageWriteImpl()
    {
        assert(mSendBufferSize != 0);

        Serialize::MessageId id = ++mRunningMessageId;

        SteamMessageHeader *header = reinterpret_cast<SteamMessageHeader *>(mSendBuffer.get());
        header->mMessageId = id;

        SteamMessagePtr message { SteamNetworkingUtils()->AllocateMessage(0) };
        message->m_pData = mSendBuffer.release();
        mSendBufferSize = 0;
        message->m_cbSize = pptr() - pbase();
        message->m_conn = mConnection;
        message->m_nFlags = k_nSteamNetworkingSend_Reliable;

        message->m_pfnFreeData = [](SteamNetworkingMessage_t *message) { delete[] static_cast<char*>(message->m_pData); };

        mSendMessages.emplace_back(std::move(message));

        return id;
    }

    Serialize::MessageId SteamStreambuf::beginMessageReadImpl()
    {
        if (!mReceivingMessage)
            return 0;

        char *data = static_cast<char *>(mReceivingMessage->m_pData);
        setg(data + sizeof(SteamMessageHeader), data + sizeof(SteamMessageHeader), data + mReceivingMessage->m_cbSize);

        SteamMessageHeader *header = reinterpret_cast<SteamMessageHeader *>(data);

        return header->mMessageId;
    }

    std::streamsize SteamStreambuf::endMessageReadImpl()
    {
        mReceivingMessage.reset();
        return message_streambuf::endMessageReadImpl();
    }

    SteamStreambuf::pos_type SteamStreambuf::seekoff(off_type off, std::ios_base::seekdir dir,
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

    SteamStreambuf::pos_type SteamStreambuf::seekpos(pos_type pos, std::ios_base::openmode mode)
    {
        if (0 <= pos && eback() + pos <= egptr()) {
            setg(eback(), eback() + pos, egptr());
            return pos;
        }
        return pos_type(off_type(-1));
    }

    void SteamStreambuf::extend()
    {
        if (mSendBufferSize == 0) {
            mSendBufferSize = 128;
            mSendBuffer = std::make_unique<char[]>(mSendBufferSize);
            setp(mSendBuffer.get(), mSendBuffer.get() + mSendBufferSize);
            pbump(sizeof(SteamMessageHeader));
        } else {
            size_t index = pptr() - pbase();
            std::unique_ptr<char[]> newBuffer = std::make_unique<char[]>(2 * mSendBufferSize);
            std::memcpy(newBuffer.get(), mSendBuffer.get(), index);
            mSendBuffer = std::move(newBuffer);
            mSendBufferSize = 2 * mSendBufferSize;
            setp(mSendBuffer.get(), mSendBuffer.get() + mSendBufferSize);
            pbump(static_cast<int>(index));
        }
    }

    Serialize::StreamResult SteamStreambuf::receiveMessages()
    {
        if (!mReceivingMessage) {
            int result = SteamNetworkingSockets()->ReceiveMessagesOnConnection(mConnection, &mReceivingMessage, 1);
            if (result < 0)
                throw 0;
        }

        return {};
    }

    Serialize::StreamResult SteamStreambuf::sendMessages()
    {
        size_t count = mSendMessages.size();

        Serialize::StreamResult result;

        if (count > 0) {
            int64_t messageNumbers[64];

            assert(count < 64);

            SteamNetworkingSockets()->SendMessages(count, &std::as_const(mSendMessages)[0], messageNumbers);

            for (size_t i = 0; i < count; ++i)
                if (messageNumbers[i] <= 0)
                    switch (-messageNumbers[i]) {
                    case k_EResultNoConnection:
                        result = std::move(STREAM_CONNECTION_LOST_ERROR());
                        break;
                    default:
                        throw 0;
                    }
                    

            for (SteamMessagePtr &msg : mSendMessages)
                msg.release();

            mSendMessages.clear();
        }

        return result;
    }

    SteamStreambuf::int_type SteamStreambuf::overflow(int c)
    {
        if (c != EOF) {
            extend();
            *pptr() = c;
            pbump(1);
            return c;
        }
        return traits_type::eof();
    }

}
}