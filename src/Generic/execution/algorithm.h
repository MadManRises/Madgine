#pragma once

#include "../pipable.h"
#include "sender.h"

namespace Engine {
namespace Execution {

    template <typename F, typename V, typename R, typename T>
    auto then(Sender<F, V, R> &&sender, T &&transform)
    {
        return make_sender<std::invoke_result_t<T, V>, R>(
            [sender { std::move(sender) }, transform { forward_capture(std::forward<T>(transform)) }]<typename Rec>(Rec &&rec) mutable {
                struct TransformedRec {
                    void set_value(R result, V value)
                    {
                        mRec.set_value(std::forward<R>(result), mTransform(std::forward<V>(value)));
                    }
                    void set_done()
                    {
                        mRec.set_done();
                    }
                    void set_error(R result)
                    {
                        mRec.set_error(std::forward<R>(result));
                    }
                    T mTransform;
                    Rec mRec;
                };
                return sender(TransformedRec { std::forward<T>(transform), std::forward<Rec>(rec) });
            });
    }

    MAKE_PIPABLE(then)

    template <typename Rec1, typename Rec2, typename V, typename R>
    struct CombinedRec {
        void set_value(R result, V value)
        {
            mRec1.set_value(result, value);
            mRec2.set_value(std::forward<R>(result), std::forward<V>(value));
        }
        void set_done()
        {
            mRec1.set_done();
            mRec2.set_done();
        }
        void set_error(R result)
        {
            mRec1.set_error(result);
            mRec2.set_error(std::forward<R>(result));
        }
        Rec1 mRec1;
        Rec2 mRec2;
    };

    template <typename Rec1, typename Rec2, typename R>
    struct CombinedRec<Rec1, Rec2, void, R> {
        void set_value(R result)
        {
            mRec1.set_value(result);
            mRec2.set_value(std::forward<R>(result));
        }
        void set_done()
        {
            mRec1.set_done();
            mRec2.set_done();
        }
        void set_error(R result)
        {
            mRec1.set_error(result);
            mRec2.set_error(std::forward<R>(result));
        }
        Rec1 mRec1;
        Rec2 mRec2;
    };

    template <typename F, typename V, typename R, typename Rec1>
    auto then_receiver(Sender<F, V, R> &&sender, Rec1 &&rec1)
    {
        return make_sender<V, R>(
            [sender { std::move(sender) }, rec1 { forward_capture(std::forward<Rec1>(rec1)) }]<typename Rec2>(Rec2 &&rec2) mutable {
                return sender(CombinedRec<Rec1, Rec2, V, R> { std::forward<Rec1>(rec1), std::forward<Rec2>(rec2) });
            });
    }

    MAKE_PIPABLE(then_receiver)

}
}