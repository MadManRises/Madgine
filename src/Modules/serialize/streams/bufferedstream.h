#pragma once

#include "serializestream.h"

#include "streamstate.h"

namespace Engine {
namespace Serialize {
    struct MODULES_EXPORT BufferedInStream : SerializeInStream {
        friend struct BufferedOutStream;

        BufferedInStream(std::unique_ptr<buffered_streambuf> &&buffer);
        BufferedInStream(BufferedInStream &&other);
        BufferedInStream(BufferedInStream &&other, SerializeManager *mgr);

        bool isMessageAvailable() const;

        void readHeader(MessageHeader &header);

        SyncManager *manager() const;

        PendingRequest *fetchRequest(TransactionId id);
        void popRequest(TransactionId id);

    protected:
        BufferedInStream(buffered_streambuf *buffer);

        buffered_streambuf &buffer() const;
    };

    struct MODULES_EXPORT BufferedOutStream : SerializeOutStream {
    public:
        BufferedOutStream(std::unique_ptr<buffered_streambuf> &&buffer);
        BufferedOutStream(BufferedOutStream &&other);
        BufferedOutStream(BufferedOutStream &&other, SerializeManager *mgr);

        void beginMessage(const SerializableUnitBase *unit, MessageType type, TransactionId id);
        void beginMessage(Command cmd);
        void endMessage();

        int sendMessages();

        template <typename... _Ty>
        void writeCommand(Command cmd, const _Ty &... args)
        {
            beginMessage(cmd);

            (void)(*this << ... << args);

            endMessage();
        }

        SyncManager *manager() const;

        TransactionId createRequest(ParticipantId requester, TransactionId requesterTransactionId, std::function<void(void *)> callback);

    protected:
        buffered_streambuf &buffer() const;
    };

    struct MODULES_EXPORT BufferedInOutStream : BufferedInStream,
                                                BufferedOutStream {
        friend struct SerializeManager;

        BufferedInOutStream(std::unique_ptr<buffered_streambuf> &&buffer);
        BufferedInOutStream(BufferedInOutStream &&other);
        BufferedInOutStream(BufferedInOutStream &&other, SerializeManager *mgr);

        StreamState state() const;
        bool isClosed() const;
        void close();

        explicit operator bool() const;

        using BufferedInStream::id;
        using BufferedInStream::buffer;
    };
} // namespace Serialize
} // namespace Engine
