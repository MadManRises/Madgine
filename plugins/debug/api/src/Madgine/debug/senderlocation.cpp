#include "../debuglib.h"

#include "senderlocation.h"

#include "debugger.h"

namespace Engine {
namespace Debug {

    SenderLocation::SenderLocation(Closure<void(CB)> state)
        : mState(std::move(state))
    {
    }

    void SenderLocation::visit(CB visitor) const
    {
        mState(std::move(visitor));
    }

}
}