#pragma once

#include "serializestream.h"

#include "Generic/lambda.h"

namespace Engine {
namespace Serialize {
    struct META_EXPORT BufferedInStream : SerializeInStream {
        friend struct BufferedOutStream;

        BufferedInStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SyncStreamData> data);
        BufferedInStream(BufferedInStream &&other);
        BufferedInStream(BufferedInStream &&other, SyncManager *mgr);

        bool isMessageAvailable();

        StreamResult readHeader(MessageHeader &header);

        SyncManager *manager() const;

        PendingRequest *fetchRequest(TransactionId id);
        void popRequest(TransactionId id);

        SyncStreamData &data() const;

    protected:
        BufferedInStream(buffered_streambuf *buffer, SyncStreamData *data);
        BufferedInStream(std::unique_ptr<buffered_streambuf> buffer, std::unique_ptr<SyncStreamData> data);

        buffered_streambuf &buffer() const;
    };

    struct META_EXPORT BufferedOutStream : SerializeOutStream {
    public:
        BufferedOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SyncStreamData> data);
        BufferedOutStream(BufferedOutStream &&other);
        BufferedOutStream(BufferedOutStream &&other, SyncManager *mgr);

        void beginMessage(const SyncableUnitBase *unit, MessageType type, TransactionId id);
        void beginMessage(Command cmd);
        void endMessage();

        BufferedOutStream &sendMessages();

        template <typename... _Ty>
        void writeCommand(Command cmd, const _Ty &... args)
        {
            beginMessage(cmd);

            (write(*this, args, "args"), ...);

            endMessage();
        }

        SyncManager *manager() const;

        TransactionId createRequest(ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback);

    protected:
        BufferedOutStream(std::unique_ptr<buffered_streambuf> buffer, std::unique_ptr<SyncStreamData> data);

        SyncStreamData &data() const;
        buffered_streambuf &buffer() const;
    };

    struct META_EXPORT BufferedInOutStream : BufferedInStream,
                                             BufferedOutStream {
        friend struct SerializeManager;

        BufferedInOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SyncStreamData> data);
        BufferedInOutStream(std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<Formatter> format, SyncManager &mgr, ParticipantId id = 0);
        BufferedInOutStream(BufferedInOutStream &&other);
        BufferedInOutStream(BufferedInOutStream &&other, SyncManager *mgr);

        explicit operator bool() const;

        using BufferedInStream::buffer;
        using BufferedInStream::data;
        using BufferedInStream::id;

    protected:
        BufferedInOutStream(std::unique_ptr<buffered_streambuf> buffer, std::unique_ptr<SyncStreamData> data);
    };
} // namespace Serialize
} // namespace Engine
