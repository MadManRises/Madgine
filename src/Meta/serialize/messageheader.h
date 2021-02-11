#pragma once

namespace Engine {
namespace Serialize {

    struct MessageHeader {
        union {
            MessageType mType;
            Command mCmd;
        };

        UnitId mObject;
        TransactionId mTransaction;
    };

}
}