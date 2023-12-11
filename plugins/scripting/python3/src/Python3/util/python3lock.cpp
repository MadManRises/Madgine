#include "../python3lib.h"

#include "python3lock.h"

#include "../python3env.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3Lock::Python3Lock(Lambda<void(std::string_view)> out)
        {
            Python3Environment::lock(std::move(out));
        }

        Python3Lock::~Python3Lock()
        {
            Python3Environment::unlock();
        }

        Python3InnerLock::Python3InnerLock()
            : mState(Python3Environment::lock())
        {
        }

        Python3InnerLock::Python3InnerLock(Python3InnerLock &&other)
            : mState(std::exchange(other.mState, std::nullopt))
        {
        }

        Python3InnerLock::~Python3InnerLock()
        {
            if (mState)
                Python3Environment::unlock(*mState);
        }

        Python3Unlock::Python3Unlock()
            : mOut(Python3Environment::unlock())
        {
        }

        Python3Unlock::~Python3Unlock()
        {
            Python3Environment::lock(std::move(mOut));
        }

        Lambda<void(std::string_view)> Python3Unlock::out()
        {
            return std::move(mOut);
        }

    }
}
}