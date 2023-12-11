#pragma once

#include "Generic/execution/virtualsender.h"
#include "Generic/lambda.h"
#include "Madgine/root/rootcomponentbase.h"
#include "Madgine/root/rootcomponentcollector.h"
#include "Meta/keyvalue/virtualscope.h"

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

        void suspend(Lambda<void()> callback);
        void resume();
        void step();

        bool alive() const;

        DebugLocation *getLocation() const;

        Lambda<void()> mPaused;
        std::deque<FrameInfo> mStack;
        bool mSingleStepping = false;
    };

    struct DebugListener {
        virtual bool pass(ContextInfo &context)
        {
            return true;
        }
        virtual void onSuspend(ContextInfo &context) { }
    };

    struct MADGINE_DEBUGGER_EXPORT Debugger : Root::RootComponent<Debugger> {

        Debugger(const Debugger &) = delete;

        using Root::RootComponent<Debugger>::RootComponent;

        virtual std::string_view key() const override;

        void yield(void *address, Lambda<void()> callback);

        DebugLocation *getLocation(void *address);
        void stepInto(void *address, std::unique_ptr<DebugLocation> location, void *parent = nullptr);
        void stepOut(void *address);

        bool pass(void *address);

        std::deque<ContextInfo> &infos();
        ContextInfo *getContext(void *address);
        ContextInfo &getOrCreateContext(void *address);
        ContextInfo &createContext(void *address, std::unique_ptr<DebugLocation> location = {});

        void addListener(DebugListener *listener);
        void removeListener(DebugListener *listener);

    protected:
        bool pass(ContextInfo &context);

    private:
        std::deque<ContextInfo> mContexts;
        std::vector<DebugListener *> mListeners;
    };

}
}

REGISTER_TYPE(Engine::Debug::Debugger)