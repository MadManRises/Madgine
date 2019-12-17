#pragma once


namespace Engine {
namespace Serialize {

	
	typedef int StateTransmissionFlags;
    enum StateTransmissionFlags_ {
        StateTransmissionFlags_SkipId = 1 << 0,
        StateTransmissionFlags_DontApplyMap = 1 << 1,
    };

}
}