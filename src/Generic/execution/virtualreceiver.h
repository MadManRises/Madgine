#pragma once

namespace Engine {
namespace Execution {

    template <typename V, typename...>
    struct VirtualReceiverBase {
    protected:
        ~VirtualReceiverBase() = default;

    public:
        virtual void set_done() = 0;
    };

    template <typename V, typename R, typename... ExtraR>
    struct VirtualReceiverBase<V, R, ExtraR...> : VirtualReceiverBase<V, ExtraR...> {
        virtual void set_value(R r, V v) = 0;        
        virtual void set_error(R r) = 0;
    };
        
    template <typename R, typename... ExtraR>
    struct VirtualReceiverBase<void, R, ExtraR...> : VirtualReceiverBase<void, ExtraR...> {
        virtual void set_value(R r) = 0;        
        virtual void set_error(R r) = 0;
    };

    
    template <typename Rec, typename Base, typename V, typename...>
    struct VirtualReceiverEx : Base {
        template <typename... Args>
        VirtualReceiverEx(Rec &&rec, Args &&...args)
            : Base(std::forward<Args>(args)...)
            , mRec(std::forward<Rec>(rec))
        {
        }
        virtual void set_done() override
        {
            mRec.set_done();
        }
        Rec mRec;
    };


    template <typename Rec, typename Base, typename V, typename R, typename... ExtraR>
    struct VirtualReceiverEx<Rec, Base, V, R, ExtraR...> : VirtualReceiverEx<Rec, Base, V, ExtraR...> {
        using VirtualReceiverEx<Rec, Base, V, ExtraR...>::VirtualReceiverEx;
        virtual void set_value(R r, V v) override
        {
            this->mRec.set_value(std::forward<R>(r), std::forward<V>(v));
        }
        virtual void set_error(R r) override
        {
            this->mRec.set_error(std::forward<R>(r));
        }
    };

    template <typename Rec, typename Base, typename R, typename... ExtraR>
    struct VirtualReceiverEx<Rec, Base, void, R, ExtraR...> : VirtualReceiverEx<Rec, Base, void, ExtraR...> {
        using VirtualReceiverEx<Rec, Base, void, ExtraR...>::VirtualReceiverEx;
        virtual void set_value(R r) override
        {
            this->mRec.set_value(std::forward<R>(r));
        }
        virtual void set_error(R r) override
        {
            this->mRec.set_error(std::forward<R>(r));
        }
    };

    template <typename Rec, typename V, typename R, typename... ExtraR>
    using VirtualReceiver = VirtualReceiverEx<Rec, VirtualReceiverBase<V, R, ExtraR...>, V, R, ExtraR...>;

}
}