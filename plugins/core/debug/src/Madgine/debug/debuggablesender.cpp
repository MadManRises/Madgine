#include "../debuglib.h"

#include "debuggablesender.h"

#include "Meta/keyvalue/valuetype.h"

namespace Engine {
namespace Execution {

	SenderLocation::SenderLocation(std::vector<StateDescriptor> state)
        : mState(std::move(state))
    {
    }

     std::string SenderLocation::toString() const 
    {
        return "Sender";
    }

     std::map<std::string_view, ValueType> SenderLocation::localVariables() const 
    {
        return {};
    }

     bool SenderLocation::wantsPause(Debug::ContinuationType type) const 
    {
        return type == Debug::ContinuationType::Error;
    }

}
}