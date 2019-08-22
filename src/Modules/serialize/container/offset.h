#pragma once

#include "../../generic/offsetptr.h"

template <typename T, size_t L>
Engine::OffsetPtr<T, Engine::Serialize::ObservableBase> ObservableOffsetInstance();

namespace Engine {
namespace Serialize {

    template <typename T, size_t L>
    struct ObservableOffsetPtr {
        static T *parent(ObservableBase *o)
        {
            return ObservableOffsetInstance<T, L>().parent(o);
        }

		static const T *parent(const ObservableBase *o)
        {
            return ObservableOffsetInstance<T, L>().parent(o);
        }

        template <typename _T = T, typename _M = ObservableBase>
        static size_t offset()
        {
            return ObservableOffsetInstance<T, L>().template offset<_T, _M>();
        }
    };

	#define DEFINE_OBSERVABLE_OFFSET(Name)                                                               \
    friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::ObservableBase> ObservableOffsetInstance<Self, __LINE__>() \
    {                                                                                 \
        return { &Self::Name };                                                       \
    }

}
}