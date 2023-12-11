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

    void Debugger::yield(void *address, Lambda<void()> callback)
    {
        ContextInfo &context = getOrCreateContext(address);
        for (DebugListener *listener : mListeners)
            listener->onSuspend(context);
        context.suspend(std::move(callback));
    }

    void Debugger::stepInto(void *address, std::unique_ptr<DebugLocation> location, void *parent)
    {
        ContextInfo *info = getContext(parent);
        if (!info) {
            createContext(address, std::move(location));
        } else {
            info->mStack.emplace_back(FrameInfo { address, std::move(location) });
        }
    }

    void Debugger::stepOut(void *address)
    {
        ContextInfo *context = getContext(address);
        assert(context);
        context->mStack.pop_back();
    }

    std::deque<ContextInfo> &Debugger::infos()
    {
        return mContexts;
    }

    ContextInfo *Debugger::getContext(void *address)
    {
        auto it = std::ranges::find_if(mContexts, [=](ContextInfo &info) { return !info.mStack.empty() && info.mStack.back().mAddress == address; });
        if (it != mContexts.end())
            return &*it;
        else
            return nullptr;
    }

    ContextInfo &Debugger::getOrCreateContext(void *address)
    {
        auto it = std::ranges::find_if(mContexts, [=](ContextInfo &info) { return !info.mStack.empty() && info.mStack.back().mAddress == address; });
        if (it != mContexts.end())
            return *it;
        else
            return createContext(address);
    }

    ContextInfo &Debugger::createContext(void *address, std::unique_ptr<DebugLocation> location)
    {
        return mContexts.emplace_back(FrameInfo { address, std::move(location) });
    }

    DebugLocation *Debugger::getLocation(void *address)
    {
        ContextInfo *context = getContext(address);
        if (!context)
            return nullptr;
        return context->getLocation();
    }

    void Debugger::addListener(DebugListener *listener)
    {
        mListeners.push_back(listener);
    }

    void Debugger::removeListener(DebugListener *listener)
    {
        std::erase(mListeners, listener);
    }

    ContextInfo::ContextInfo(FrameInfo info)
    {
        mStack.emplace_back(std::move(info));
    }

    void ContextInfo::suspend(Lambda<void()> callback)
    {
        mPaused = std::move(callback);
    }

    void ContextInfo::resume()
    {
        assert(mPaused);
        mSingleStepping = false;
        Lambda<void()> receiver = std::move(mPaused);
        receiver();
    }

    void ContextInfo::step()
    {
        assert(mPaused);
        mSingleStepping = true;
        Lambda<void()> receiver = std::move(mPaused);
        receiver();
    }

    bool ContextInfo::alive() const
    {
        return !mStack.empty();
    }

    DebugLocation *ContextInfo::getLocation() const
    {
        if (mStack.empty())
            return nullptr;
        return mStack.back().mLocation.get();
    }

    bool Debugger::pass(void *address)
    {
        ContextInfo *context = getContext(address);
        if (!context)
            return true;
        return pass(*context);
    }

    bool Debugger::pass(ContextInfo &context)
    {
        bool pass = true;

        for (DebugListener *listener : mListeners) {
            pass &= listener->pass(context);
        }

        return pass;
    }

}
}
