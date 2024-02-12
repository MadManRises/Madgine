#pragma once

#include "Generic/callable_view.h"
#include "Generic/closure.h"
#include "Generic/execution/statedescriptor.h"
#include "Generic/execution/stop_callback.h"
#include "Generic/execution/virtualsender.h"
#include "Generic/forward_capture.h"
#include "Madgine/root/rootcomponentbase.h"
#include "Madgine/root/rootcomponentcollector.h"

namespace Engine {
namespace Debug {

    enum class ContinuationMode {
        Resume,
        Step,
        Abort,
        None
    };

    struct MADGINE_DEBUGGER_EXPORT ParentLocation {

        DebugLocation *currentLocation() const;

        DebugLocation *mChild = nullptr;
        ContextInfo *mContext = nullptr;
    };

    struct MADGINE_DEBUGGER_EXPORT DebugLocation : ParentLocation {
        virtual ~DebugLocation() = default;
        virtual std::string toString() const = 0;
        virtual std::map<std::string_view, ValueType> localVariables() const = 0;
        virtual bool wantsPause() const = 0;

        void stepInto(ParentLocation *parent);
        void stepOut(ParentLocation *parent);
        template <typename F>
        void yield(F &&callback, std::stop_token st)
        {
            mContext->suspend(std::forward<F>(callback), std::move(st));
        }
        bool pass();
        template <typename F, typename... Args>
        void pass(F &&callback, std::stop_token st, Args &&...args)
        {
            if (pass()) {
                std::forward<F>(callback)(ContinuationMode::Resume, std::forward<Args>(args)...);
            } else {
                yield([callback { forward_capture(std::forward<F>(callback)) }, args = std::tuple<Args...> { std::forward<Args>(args)... }](ContinuationMode mode) mutable {
                    TupleUnpacker::invokeExpand(std::forward<F>(callback), mode, std::move(args));
                },
                    std::move(st));
            }
        }
    };

    struct MADGINE_DEBUGGER_EXPORT ContextInfo : ParentLocation {
        ContextInfo()
            : ParentLocation { nullptr, this }       
            , mStopCallback(finally_cb { this })
        {
        }

        void suspend(Closure<void(ContinuationMode)> callback, std::stop_token st);
        void continueExecution(ContinuationMode mode);

        void resume();
        void step();
        void abort();

        bool alive() const;
        bool isPaused() const;

        struct stop_cb {
            ContextInfo *mContext;
            bool operator()() const;
        };

        struct finally_cb {
            ContextInfo *mContext;
            void operator()(ContinuationMode mode) const;
            void operator()(Execution::cancelled_t) const;
        };


        mutable std::mutex mMutex;
    private:
        Closure<void(ContinuationMode)> mCallback;
        Execution::stop_callback<stop_cb, finally_cb> mStopCallback;
        std::atomic<int> mPaused = 0;
    };

    struct DebugListener {
        virtual bool pass(DebugLocation *location)
        {
            return true;
        }
        virtual void onSuspend(ContextInfo &context) { }
    };

    struct MADGINE_DEBUGGER_EXPORT Debugger : Root::RootComponent<Debugger> {

        friend struct ContextInfo;

        Debugger(const Debugger &) = delete;

        using Root::RootComponent<Debugger>::RootComponent;

        virtual std::string_view key() const override;

        std::deque<ContextInfo> &infos();
        ContextInfo &createContext();

        void addListener(DebugListener *listener);
        void removeListener(DebugListener *listener);

        bool pass(DebugLocation *location);

    private:
        std::deque<ContextInfo> mContexts;
        std::vector<DebugListener *> mListeners;
    };

}
}

REGISTER_TYPE(Engine::Debug::Debugger)