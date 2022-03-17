#pragma once

#include "formattedserializestream.h"

#include "Generic/lambda.h"

#include "Generic/nulledptr.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT FormattedBufferedStream : FormattedSerializeStream {

        FormattedBufferedStream(std::unique_ptr<Formatter> format, std::unique_ptr<std::basic_streambuf<char>> buffer, std::unique_ptr<SyncStreamData> data);
        FormattedBufferedStream(FormattedBufferedStream &&other) = default;
        FormattedBufferedStream(FormattedBufferedStream &&other, SyncManager *mgr);

        FormattedBufferedStream &operator=(FormattedBufferedStream &&) = default;

        void beginMessageWrite();
        void endMessageWrite();
        
        struct META_EXPORT MessageReadMarker {
            MessageReadMarker(Formatter *formatter = nullptr)
                : mFormatter(formatter)
            {
            }
            MessageReadMarker(MessageReadMarker &&other) = default;
            ~MessageReadMarker();

            MessageReadMarker &operator=(MessageReadMarker &&other);

            StreamResult end();

            explicit operator bool() const;

            NulledPtr<Formatter> mFormatter;            
        };

        StreamResult beginMessageRead(MessageReadMarker &msg);

        FormattedBufferedStream &sendMessages();


        StreamResult beginHeaderRead();
        StreamResult endHeaderRead();

        void beginHeaderWrite();
        void endHeaderWrite();

        TransactionId createRequest(ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback);

        PendingRequest *fetchRequest(TransactionId id);
        void popRequest(TransactionId id);
    };

}
}