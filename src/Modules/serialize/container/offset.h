#pragma once

#include "../../generic/offsetptr.h"

template <typename T, size_t L>
Engine::OffsetPtr<T, Engine::Serialize::ObservableBase> ObservableOffsetInstance();

template <typename T, size_t L>
Engine::OffsetPtr<T, Engine::Serialize::Serializable> SerializableOffsetInstance();

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

	template <typename T, size_t L>
    struct SerializableOffsetPtr {
        static T *parent(Serializable *o)
        {
            return SerializableOffsetInstance<T, L>().parent(o);
        }

        static const T *parent(const Serializable *o)
        {
            return SerializableOffsetInstance<T, L>().parent(o);
        }

        template <typename _T = T, typename _M = Serializable>
        static size_t offset()
        {
            return SerializableOffsetInstance<T, L>().template offset<_T, _M>();
        }
    };

#define DEFINE_SERIALIZABLE_OFFSET(Name)                                                                               \
    friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::Serializable> SerializableOffsetInstance<Self, __LINE__>() \
    {                                                                                                                \
        return { &Self::Name };                                                                                      \
    }

	template <typename T, size_t L>
    struct CombinedOffsetPtr : SerializableOffsetPtr<T, L>, ObservableOffsetPtr<T, L> {
		
		template <typename Ty>
		static T *parent(Ty *o)
            {
                    if constexpr (std::is_convertible_v<Ty &, Serializable &>)
                    return SerializableOffsetPtr<T, L>::parent(o);
                    else
                        return ObservableOffsetPtr<T, L>::parent(o);
            }

            template <typename Ty>
            static const T *parent(const Ty *o)
            {
                if constexpr (std::is_convertible_v<Ty &, Serializable &>)
                    return SerializableOffsetPtr<T, L>::parent(o);
                else
                    return ObservableOffsetPtr<T, L>::parent(o);
            }

				
		template <typename _T, typename _M>
            static size_t offset()
            {
                if constexpr (std::is_convertible_v<_M&, ObservableBase&>)
                    return ObservableOffsetPtr<T, L>::template offset<_T, _M>();
                else
                    return SerializableOffsetPtr<T, L>::template offset<_T, _M>();
            }

    };

	#define DEFINE_COMBINED_OFFSET(Name) DEFINE_SERIALIZABLE_OFFSET(Name) DEFINE_OBSERVABLE_OFFSET(Name)

}
}