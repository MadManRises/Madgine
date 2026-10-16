
#include "Generic/closure.h"

#include "Generic/execution/statedescriptor.h"

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
        ContinuationList(const Debug::ContextInfo &context, ControlButton button);
        ContinuationList(const ContinuationList &) = delete;
        ~ContinuationList();

        ContinuationList &operator=(const ContinuationList &) = delete;

        void breakpoint(float y0, float y1, size_t line, TypedPtr location);
        void lazyBreakpoint(const Execution::State::Breakpoint &bp, size_t line, TypedPtr location);
        void marker();
        void flush(float y0, float y1);

        void controls(Debug::Continuation &continuation, Closure<void()> stopCallback, bool isActive = true);

    private:
        const Debug::ContextInfo &mContext;

        ControlButton mButton;
        std::vector<std::tuple<Debug::Continuation &, Debug::ContinuationMode, Closure<void()>>> mContinuations;

        IndexType<size_t> mBreakpoint;
        bool mIsMarker = false;
        Debug::Continuation *mContinuation = nullptr;
        IndexType<size_t> *mLineFeedback = nullptr;
        Closure<void()> mStopCallback;
        bool mIsActive = false;
        TypedPtr mLocation;
    };

}
}