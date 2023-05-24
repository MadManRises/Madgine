#include "../python3lib.h"

#include "python3lock.h"

#include "../python3env.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3Lock::Python3Lock(std::streambuf *o)
        {
            Python3Environment::lock(o);
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
            : mStream(Python3Environment::unlock())
        {
        }

        Python3Unlock::~Python3Unlock()
        {
            Python3Environment::lock(mStream);
        }

        std::streambuf *Python3Unlock::out() const
        {
            return mStream;
        }

    }
}
}