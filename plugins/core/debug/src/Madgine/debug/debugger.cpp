#include "../debuglib.h"

#include "debugger.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"

UNIQUECOMPONENT(Engine::Debug::Debugger)

METATABLE_BEGIN(Engine::Debug::Debugger)
METATABLE_END(Engine::Debug::Debugger)

namespace Engine {
namespace Debug {

    std::string_view Debugger::key() const
    {
        return "Debugger";
    }

    void DebugLocation::stepInto(ParentLocation *parent)
    {
        mContext = parent->mContext;
        std::unique_lock guard { mContext->mMutex };
        assert(!parent->mChild);
        parent->mChild = this;
    }

    void DebugLocation::stepOut(ParentLocation *parent)
    {
        std::unique_lock guard { mContext->mMutex };
        assert(parent->mChild == this);
        parent->mChild = nullptr;
    }

    bool DebugLocation::pass()
    {
        return !wantsPause() && Debugger::getSingleton().pass(this);
    }

    std::deque<ContextInfo> &Debugger::infos()
    {
        return mContexts;
    }

    ContextInfo &Debugger::createContext()
    {
        return mContexts.emplace_back();
    }

    void Debugger::addListener(DebugListener *listener)
    {
        mListeners.push_back(listener);
    }

    void Debugger::removeListener(DebugListener *listener)
    {
        std::erase(mListeners, listener);
    }

    void ContextInfo::suspend(Closure<void(ContinuationMode)> callback, std::stop_token st)
    {
        for (DebugListener *listener : Debugger::getSingleton().mListeners)
            listener->onSuspend(*this);
        mCallback = std::move(callback);
        int initialState = 0;
        mPaused.compare_exchange_strong(initialState, 1);
        assert(initialState == 0);
        mStopCallback.start(std::move(st), stop_cb { this });
        initialState = 1;
        mPaused.compare_exchange_strong(initialState, 2);
    }

    void ContextInfo::continueExecution(ContinuationMode mode)
    {
        int initialState = 2;
        if (mPaused.compare_exchange_strong(initialState, 3)) {
            mStopCallback.finish(mode);
        }
    }

    void ContextInfo::resume()
    {
        continueExecution(ContinuationMode::Resume);
    }

    void ContextInfo::step()
    {
        continueExecution(ContinuationMode::Step);
    }

    void ContextInfo::abort()
    {
        continueExecution(ContinuationMode::Abort);
    }

    bool ContextInfo::alive() const
    {
        return mChild;
    }

    bool ContextInfo::isPaused() const
    {
        return mPaused == 2;
    }

    bool Debugger::pass(DebugLocation *location)
    {
        bool pass = true;

        for (DebugListener *listener : mListeners) {
            pass &= listener->pass(location);
        }

        return pass;
    }

    bool ContextInfo::stop_cb::operator()() const
    {
        int initialState = 1;
        if (mContext->mPaused.compare_exchange_strong(initialState, 3))
            return true;
        initialState = 2;
        return mContext->mPaused.compare_exchange_strong(initialState, 3);
    }

    void ContextInfo::finally_cb::operator()(ContinuationMode mode) const
    {
        assert(mContext->mPaused == 3);
        Closure<void(ContinuationMode)> callback = std::move(mContext->mCallback);
        mContext->mPaused = 0;
        callback(mode);
    }

    void ContextInfo::finally_cb::operator()(Execution::cancelled_t) const
    {
        operator()(ContinuationMode::Abort);
    }

    DebugLocation *ParentLocation::currentLocation() const
    {
        if (!mChild)
            return nullptr;
        if (!mChild->mChild)
            return mChild;
        return mChild->currentLocation();
    }
}
}
