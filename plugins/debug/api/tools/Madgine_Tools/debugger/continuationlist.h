
#include "Generic/closure.h"

namespace Engine {
namespace Tools {

    enum class ControlButton {
        NONE,
        PLAY,
        STEPOVER,
        STEPINTO,
        PAUSE,
        STOP
    };

    struct MADGINE_DEBUGGER_TOOLS_EXPORT ContinuationList {
        ContinuationList(ControlButton button);
        ContinuationList(const ContinuationList &) = delete;
        ~ContinuationList();

        ContinuationList &operator=(const ContinuationList &) = delete;

        void controls(Debug::Continuation &continuation, Closure<void()> stopCallback);

    private:
        ControlButton mButton;
        std::vector<std::tuple<Debug::Continuation &, Debug::ContinuationMode, Closure<void()>>> mContinuations;
    };

}
}