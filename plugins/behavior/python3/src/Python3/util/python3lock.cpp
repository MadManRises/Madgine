#include "../python3lib.h"

#include "python3lock.h"

#include "../python3env.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3Lock::Python3Lock(Closure<void(std::string_view)> out, std::stop_token st)
        {
            Python3Environment::lock(std::move(out), std::move(st));
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
            : mState(Python3Environment::unlock())
        {
        }

        Python3Unlock::~Python3Unlock()
        {
            Python3Environment::lock(std::move(mState.first), std::move(mState.second));
        }

        Closure<void(std::string_view)> Python3Unlock::out()
        {
            return std::move(mState.first);
        }

        std::stop_token Python3Unlock::st()
        {
            return std::move(mState.second);
        }

    }
}
}