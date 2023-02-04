#pragma once

namespace Engine {
namespace Execution {

    template <typename V, typename R>
    struct VirtualReceiverBase {
    protected:
        ~VirtualReceiverBase() = default;

    public:
        virtual void set_value(R r, V v) = 0;
        virtual void set_done() = 0;
        virtual void set_error(R r) = 0;
    };

    template <typename Rec, typename V, typename R, typename Base = VirtualReceiverBase<V, R>>
    struct VirtualReceiver : Base {
        template <typename... Args>
        VirtualReceiver(Rec &&rec, Args &&...args)
            : Base(std::forward<Args>(args)...)
            , mRec(std::forward<Rec>(rec))
        {
        }
        virtual void set_value(R r, V v) override
        {
            mRec.set_value(std::forward<R>(r), std::forward<V>(v));
        }
        virtual void set_done() override
        {
            mRec.set_done();
        }
        virtual void set_error(R r) override
        {
            mRec.set_error(std::forward<R>(r));
        }
        Rec mRec;
    };

}
}