#include "../debuglib.h"

#include "contextinfo.h"

#include "Generic/execution/stop_source.h"

#include "Meta/reflect/metatable_impl.h"

#include "continuation.h"
#include "debugger.h"
#include "debuglistener.h"

METATABLE_BEGIN(Engine::Debug::ContextInfo)
    READONLY_PROPERTY(Alive, alive)
METATABLE_END(Engine::Debug::ContextInfo)

namespace Engine {
namespace Debug {

    bool ContextInfo::wantsPause(TypedPtr location, ContinuationType type, IndexType<size_t> line)
    {
        bool pause = (line && (type == Debug::ContinuationType::Error || getBreakpoint(location.ptr(), line)));

        for (DebugListener *listener : Debugger::getSingleton().mListeners) {
            pause |= listener->wantsPause(*this, location, type, line);
        }

        return pause;
    }

    void ContextInfo::suspend(TypedPtr location, ContinuationType type)
    {
        for (DebugListener *listener : Debugger::getSingleton().mListeners)
            listener->onSuspend(*this, location, type);
    }

    bool ContextInfo::alive() const
    {
        return static_cast<bool>(mChild);
    }

    bool ContextInfo::isPaused() const
    {
        return false;
    }

    void ContextInfo::setBreakpoint(const void *location, size_t index, bool set) const
    {
        auto &data = mDebugLocations[location];

        if (data.mBreakpoints.size() <= index) {
            data.mBreakpoints.resize(index + 1);
        }
        data.mBreakpoints[index] = set;
    }

    bool ContextInfo::getBreakpoint(const void *location, size_t index) const
    {
        auto it = mDebugLocations.find(location);
        if (it == mDebugLocations.end())
            return false;
        return it->second.mBreakpoints.size() > index && it->second.mBreakpoints[index];
    }

    void ContextInfo::stepInto(TypedPtr &parent, TypedPtr location)
    {
        std::unique_lock lock { mMutex };
        assert(!parent);
        parent = location;
    }

    void ContextInfo::stepOut(TypedPtr &parent, TypedPtr location)
    {
        std::unique_lock lock { mMutex };
        assert(parent == location);
        parent = nullptr;
    }

}
}