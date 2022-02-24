#pragma once

namespace Engine {
namespace Serialize {

    typedef int StateTransmissionFlags;
    enum StateTransmissionFlags_ {
        StateTransmissionFlags_SkipId = 1 << 0,
        StateTransmissionFlags_ApplyMap = 1 << 1,
        StateTransmissionFlags_Activation = 1 << 2,
    };

}
}