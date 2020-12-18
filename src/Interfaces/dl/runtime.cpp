#include "../interfaceslib.h"

#include "runtime.h"

#include "../filesystem/api.h"

namespace Engine {
namespace Dl {
    
    Filesystem::FileQuery listSharedLibraries()
    {
#if LINUX || OSX || IOS || WINDOWS
        return Filesystem::FileQuery { Filesystem::executablePath() };
#elif ANDROID
        return SharedLibraryQuery { Filesystem::Path { "/data/data/com.Madgine.MadgineLauncher/lib" } }; //TODO
#elif EMSCRIPTEN
        std::terminate();
#else
#    error "Unsupported Platform!"
#endif
    }

}
}