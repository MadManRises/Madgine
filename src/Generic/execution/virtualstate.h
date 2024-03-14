#pragma once

#include "Generic/type_pack.h"

#include "Generic/execution/concepts.h"

namespace Engine {
namespace Execution {

    template <typename R, typename... V>
    struct VirtualReceiverBaseEx;

    template <typename... V>
    struct VirtualReceiverBaseEx<type_pack<>, V...> {
        ~VirtualReceiverBaseEx() = default;

    public:
        virtual void set_done() = 0;
        virtual void set_value(V... v) = 0;
    };

    template <typename R, typename... ExtraR, typename... V>
    struct VirtualReceiverBaseEx<type_pack<R, ExtraR...>, V...> : VirtualReceiverBaseEx<type_pack<ExtraR...>, V...> {
        virtual void set_error(R r) = 0;
    };

    template <typename R, typename... V>
    using VirtualReceiverBase = VirtualReceiverBaseEx<make_type_pack_t<R>, V...>;

    template <typename Rec, typename Base, typename R, typename... V>
    struct VirtualStateEx;

    template <typename _Rec, typename Base, typename... V>
    struct VirtualStateEx<_Rec, Base, type_pack<>, V...> : Base {

        using Rec = _Rec;
        
        template <typename... Args>
        VirtualStateEx(Rec &&rec, Args &&...args)
            : Base(std::forward<Args>(args)...)
            , mRec(std::forward<Rec>(rec))
        {
        }
        virtual void set_done() override
        {
            mRec.set_done();
        }
        virtual void set_value(V... v) override
        {
            this->mRec.set_value(std::forward<V>(v)...);
        }

        friend Rec& tag_invoke(Execution::get_receiver_t, VirtualStateEx& state) {
            return state.mRec;
        }

        Rec mRec;
    };

    template <typename Rec, typename Base, typename R, typename... ExtraR, typename... V>
    struct VirtualStateEx<Rec, Base, type_pack<R, ExtraR...>, V...> : VirtualStateEx<Rec, Base, type_pack<ExtraR...>, V...> {
        
        using VirtualStateEx<Rec, Base, type_pack<ExtraR...>, V...>::VirtualStateEx;

        using result_type = R;

        virtual void set_error(R r) override
        {
            this->mRec.set_error(std::forward<R>(r));
        }
    };

    template <typename Rec, typename R, typename... V>
    using VirtualState = VirtualStateEx<Rec, VirtualReceiverBase<R, V...>, make_type_pack_t<R>, V...>;

}
}