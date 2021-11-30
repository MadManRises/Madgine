#pragma once

#include "serializestream.h"

#include "Generic/lambda.h"

namespace Engine {
namespace Serialize {
    struct META_EXPORT BufferedInStream : SerializeInStream {
        friend struct BufferedOutStream;

        BufferedInStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<BufferedStreamData> data);
        BufferedInStream(BufferedInStream &&other);
        BufferedInStream(BufferedInStream &&other, SerializeManager *mgr);

        bool isMessageAvailable();

        StreamResult readHeader(MessageHeader &header);

        SyncManager *manager() const;

        PendingRequest *fetchRequest(TransactionId id);
        void popRequest(TransactionId id);

    protected:
        BufferedInStream(buffered_streambuf *buffer, BufferedStreamData *data);
        BufferedInStream(std::unique_ptr<buffered_streambuf> buffer, std::unique_ptr<BufferedStreamData> data);

        BufferedStreamData &data() const;
        buffered_streambuf &buffer() const;
    };

    struct META_EXPORT BufferedOutStream : SerializeOutStream {
    public:
        BufferedOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<BufferedStreamData> data);
        BufferedOutStream(BufferedOutStream &&other);
        BufferedOutStream(BufferedOutStream &&other, SerializeManager *mgr);

        void beginMessage(const SyncableUnitBase *unit, MessageType type, TransactionId id);
        void beginMessage(Command cmd);
        void endMessage();

        BufferedOutStream &sendMessages();

        template <typename... _Ty>
        void writeCommand(Command cmd, const _Ty &... args)
        {
            beginMessage(cmd);

            (void)(*this << ... << args);

            endMessage();
        }

        SyncManager *manager() const;

        TransactionId createRequest(ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback);

    protected:
        BufferedOutStream(std::unique_ptr<buffered_streambuf> buffer, std::unique_ptr<BufferedStreamData> data);

        BufferedStreamData &data() const;
        buffered_streambuf &buffer() const;
    };

    struct META_EXPORT BufferedInOutStream : BufferedInStream,
                                             BufferedOutStream {
        friend struct SerializeManager;

        BufferedInOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<BufferedStreamData> data);
        BufferedInOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<Formatter> format, SyncManager &mgr, ParticipantId id = 0);
        BufferedInOutStream(BufferedInOutStream &&other);
        BufferedInOutStream(BufferedInOutStream &&other, SerializeManager *mgr);

        explicit operator bool() const;

        using BufferedInStream::buffer;
        using BufferedInStream::data;
        using BufferedInStream::id;

    protected:
        BufferedInOutStream(std::unique_ptr<buffered_streambuf> buffer, std::unique_ptr<BufferedStreamData> data);
    };
} // namespace Serialize
} // namespace Engine
