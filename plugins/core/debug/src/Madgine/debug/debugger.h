#pragma once

#include "Madgine/root/rootcomponentbase.h"
#include "Madgine/root/rootcomponentcollector.h"
#include "Meta/keyvalue/virtualscope.h"
#include "Generic/execution/virtualsender.h"

namespace Engine {
namespace Debug {

    struct DebugLocation {
        virtual ~DebugLocation() = default;
        virtual std::string toString() const = 0;
        virtual std::map<std::string_view, ValueType> localVariables() const = 0;
    };

    struct FrameInfo {

        void *mAddress;
        std::unique_ptr<DebugLocation> mLocation;
    };

    struct MADGINE_DEBUGGER_EXPORT ContextInfo {
        ContextInfo(FrameInfo info);
        ContextInfo(const ContextInfo &) = delete;
        ContextInfo(ContextInfo &&) = default;

        ContextInfo &operator=(const ContextInfo &) = delete;
        ContextInfo &operator=(ContextInfo &&) = default;

        void suspend(Execution::VirtualReceiverBase<type_pack<>> &receiver);
        void resume();
        
        bool alive() const;
        
        Execution::VirtualReceiverBase<type_pack<>> *mPaused = nullptr;
        std::deque<FrameInfo> mStack;
    };

    struct MADGINE_DEBUGGER_EXPORT Debugger : Root::RootComponent<Debugger> {

        Debugger(const Debugger &) = delete;

        using Root::RootComponent<Debugger>::RootComponent;

        virtual std::string_view key() const override;

        void yield(Execution::VirtualReceiverBase<type_pack<>> &receiver, void *address);

        struct YieldState : Execution::VirtualReceiverBase<type_pack<>> {

            YieldState(Debugger *self, void *address)
                : mSelf(self)
                , mAddress(address)
            {
            }

            void start()
            {
                mSelf->yield(*this, mAddress);
            }

            Debugger *mSelf;
            void *mAddress;
        };
        
        auto yield(void *address)
        {
            return Execution::make_virtual_sender<YieldState, type_pack<>>(this, std::move(address));
        }

        DebugLocation *getLocation(void *address);
        void stepInto(void *address, std::unique_ptr<DebugLocation> location, void *parent = nullptr);
        void stepOut(void *address);

        std::deque<ContextInfo> &infos();
        ContextInfo *getContext(void *address);
        ContextInfo &getOrCreateContext(void *address);
        ContextInfo &createContext(void *address, std::unique_ptr<DebugLocation> location = {});

    private:
        std::deque<ContextInfo> mContexts;
    };

}
}

REGISTER_TYPE(Engine::Debug::Debugger)