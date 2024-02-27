

#if defined(Im3D_EXPORTS)
#    define MADGINE_PLAY_SERVICES_EXPORT DLL_EXPORT
#else
#    define MADGINE_PLAY_SERVICES_EXPORT DLL_IMPORT
#endif

#include "Generic/execution/virtualsender.h"

namespace PlayServices {

MADGINE_PLAY_SERVICES_EXPORT void setup();

namespace Achievements {
    MADGINE_PLAY_SERVICES_EXPORT void unlockImpl(Engine::Execution::VirtualReceiverBase<std::string> &rec, std::string_view id);
    inline auto unlock(std::string_view id) {
        return Engine::Execution::make_simple_virtual_sender<std::string>(&unlockImpl, std::move(id)); 
    }
}

}
