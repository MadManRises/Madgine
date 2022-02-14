#pragma once

#include "formattedserializestream.h"

#include "Generic/lambda.h"

namespace Engine {
namespace Serialize {

    struct FormattedBufferedStream : FormattedSerializeStream {

        void beginMessage(const SyncableUnitBase *unit, MessageType type, TransactionId id);
        void beginMessage(Command cmd);
        void endMessage();

        FormattedBufferedStream &sendMessages();

        bool isMessageAvailable();

        StreamResult readHeader(MessageHeader &header);

        TransactionId createRequest(ParticipantId requester, TransactionId requesterTransactionId, Lambda<void(void *)> callback);

        PendingRequest *fetchRequest(TransactionId id);
        void popRequest(TransactionId id);

        template <typename... _Ty>
        void writeCommand(Command cmd, const _Ty &...args)
        {
            beginMessage(cmd);

            (write(*this, args, "args"), ...);

            endMessage();
        }
    };

}
}