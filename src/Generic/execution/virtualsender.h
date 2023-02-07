#pragma once

#include "virtualreceiver.h"

namespace Engine {
namespace Execution {

    template <typename V, typename R, typename... ExtraR, typename F, typename... Args>
    auto make_virtual_sender(F &&f, Args &&...args)
    {
        return make_sender<V, R>(
            [f { forward_capture(std::forward<F>(f)) }, args = std::tuple<Args...> { std::forward<Args>(args)... }]<typename Rec>(Rec &&rec) mutable {
                struct state {
                    void start()
                    {
                        TupleUnpacker::invokeExpand(std::forward<F>(mF), mRec, std::move(mArgs));
                    }

                    F mF;
                    VirtualReceiver<Rec, V, R, ExtraR...> mRec;
                    std::tuple<Args...> mArgs;
                };
                return state { std::forward<F>(f), std::forward<Rec>(rec), std::move(args) };
            });
    }

}
}