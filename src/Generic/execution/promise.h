#pragma once

#include "../withresultfuture.h"
#include "execution.h"

namespace Engine {
namespace Execution {

    template <typename P, typename Rec, typename R, typename... V>
    struct PromiseRec {
        void set_value(V... value)
        {
            mPromise.set_value({ value... });
            mRec.set_value(std::forward<V>(value)...);
        }
        void set_done()
        {
            mRec.set_done();
        }
        void set_error(R result)
        {
            mPromise.set_value(result);
            mRec.set_error(std::forward<R>(result));
        }
        P mPromise;
        Rec mRec;
    };

    template <typename F, typename R, typename... V, typename P>
    auto then_promise(Sender<F, R, V...> &&sender, P &&promise)
    {
        return make_sender<R, V...>(
            [sender { std::move(sender) }, promise { forward_capture(std::forward<P>(promise)) }]<typename Rec>(Rec &&rec) mutable {
                return sender(PromiseRec<P, Rec, R, V...> { std::forward<P>(promise), std::forward<Rec>(rec) });
            });
    }

    template <typename F, typename R, typename... V>
    WithResultFuture<R, V...> detach_with_future(Sender<F, R, V...> &&sender)
    {
        std::promise<WithResult<R, V...>> p;
        WithResultFuture<R, V...> f { p.get_future() };
        detach(then_promise(std::move(sender), std::move(p)));
        return f;
    }

}
}