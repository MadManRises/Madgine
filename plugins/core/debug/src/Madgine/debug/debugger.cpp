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

    void Debugger::yield(Execution::VirtualReceiverBase<type_pack<>> &receiver, void *address)
    {
        ContextInfo &context = getOrCreateContext(address);
        if (false)
            context.suspend(receiver);
        else
            receiver.set_value();
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
        if (context->mStack.empty())
            return nullptr;
        return context->mStack.back().mLocation.get();
    }

    ContextInfo::ContextInfo(FrameInfo info)
    {
        mStack.emplace_back(std::move(info));
    }

    void ContextInfo::suspend(Execution::VirtualReceiverBase<type_pack<>> &receiver)
    {
        mPaused = &receiver;
    }

    void ContextInfo::resume()
    {
        assert(mPaused);
        Execution::VirtualReceiverBase<type_pack<>> *receiver = std::exchange(mPaused, nullptr);
        receiver->set_value();
    }

    bool ContextInfo::alive() const
    {
        return !mStack.empty();
    }

}
}
