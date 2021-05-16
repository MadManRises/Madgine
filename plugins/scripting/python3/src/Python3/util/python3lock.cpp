#include "../python3lib.h"

#include "python3lock.h"

#include "../python3env.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3Lock::Python3Lock(std::streambuf *o) : mHolding(true)
        {
            Python3Environment::lock(o);
        }

        Python3Lock::Python3Lock(Python3Lock &&other)
            : mHolding(std::exchange(other.mHolding, false))
        {
        }

        Python3Lock::~Python3Lock()
        {
            if (mHolding)
                Python3Environment::unlock();
        }

    }
}
}