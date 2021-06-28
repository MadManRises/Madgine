#include "../python3lib.h"

#include "python3lock.h"

#include "../python3env.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3Lock::Python3Lock(std::streambuf *o)
            : mState(Python3Environment::lock(o))
        {
        }

        Python3Lock::Python3Lock(Python3Lock &&other)
            : mState(std::exchange(other.mState, std::nullopt))
        {
        }

        Python3Lock::~Python3Lock()
        {
            if (mState)
                Python3Environment::unlock(*mState);
        }

    }
}
}