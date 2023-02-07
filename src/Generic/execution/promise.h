#pragma once

#include "execution.h"
#include "../withresultfuture.h"

namespace Engine {
namespace Execution {

    template <typename Rec, typename V, typename R, typename P>
    struct PromiseRec {
        void set_value(R result, V value)
        {
            mPromise.set_value({ result, value });
            mRec.set_value(std::forward<R>(result), std::forward<V>(value));
        }
        void set_done()
        {
            mRec.set_done();
        }
        void set_error(R result)
        {
            mPromise.set_value({ result });
            mRec.set_error(std::forward<R>(result));
        }
        P mPromise;
        Rec mRec;
    };

    template <typename Rec, typename R, typename P>
    struct PromiseRec<Rec, void, R, P> {
        void set_value(R result)
        {
            mPromise.set_value({ result });
            mRec.set_value(std::forward<R>(result));
        }
        void set_done()
        {
            mRec.set_done();
        }
        void set_error(R result)
        {
            mPromise.set_value({ result });
            mRec.set_error(std::forward<R>(result));
        }
        P mPromise;
        Rec mRec;
    };

	template <typename F, typename V, typename R, typename P>
    auto then_promise(Sender<F, V, R> &&sender, P &&promise)
    {
        return make_sender<V, R>(
            [sender { std::move(sender) }, promise { forward_capture(std::forward<P>(promise)) }]<typename Rec>(Rec &&rec) mutable {
                return sender(PromiseRec<Rec, V, R, P> { std::forward<P>(promise), std::forward<Rec>(rec) });
            });
    }

    template <typename F, typename V, typename R>
    WithResultFuture<V, R> detach_with_future(Sender<F, V, R>&& sender) {
        std::promise<WithResult<V, R>> p;
        WithResultFuture<V, R> f { p.get_future() };
        detach(then_promise(std::move(sender), std::move(p)));
        return f;
    }

}
}