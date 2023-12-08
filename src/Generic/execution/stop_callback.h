#pragma once

#include "../manuallifetime.h"
#include "gate.h"

namespace Engine {
namespace Execution {

    struct cancelled_t {
    };
    constexpr cancelled_t cancelled;

    template <typename stop_cb, typename cleanup_cb>
    struct stop_callback : cleanup_cb {

        template <typename CC>
        stop_callback(CC &&cc)
            : cleanup_cb(std::forward<CC>(cc))
        {            
        }

        template <typename SC>
        void start(std::stop_token st, SC &&sc)
        {
            construct(mCallback, std::move(st), inner_callback { *this, std::forward<SC>(sc) });
        }

        template <typename... Args>
        void finish(Args &&...args)
        {
            if (mGate.pass([this]() {
                    destruct(mCallback);
                })) {
                cleanup_cb::operator()(std::forward<Args>(args)...);
            }
        }

    private:
        struct inner_callback : stop_cb {

            template <typename C>
            inner_callback(stop_callback &callback, C &&c)
                : stop_cb(std::forward<C>(c))
                , mCallback(callback)
            {
            }

            void operator()()
            {
                stop_callback &cb = mCallback;
                if (mCallback.mGate.pass([&]() {
                        stop_cb::operator()();
                        destruct(cb.mCallback);
                    })) {
                    cb(cancelled);
                }
            }

            stop_callback &mCallback;
        };

        Gate mGate;
        ManualLifetime<std::stop_callback<inner_callback>> mCallback = std::nullopt;
    };
}
}