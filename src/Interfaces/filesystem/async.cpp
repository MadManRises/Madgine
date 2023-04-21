#include "../interfaceslib.h"

#include "async.h"

namespace Engine {
namespace Filesystem {

    AsyncFileRead readFileAsync(const Path &path)
    {
        return { path };
    }

}
}
