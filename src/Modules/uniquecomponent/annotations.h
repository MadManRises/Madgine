#pragma once

#include "Generic/replace.h"

namespace Engine {
namespace UniqueComponent {

    template <typename... Annotations>
    struct GroupedAnnotation : Annotations...{
        template <typename T>
        GroupedAnnotation(type_holder_t<T> t)
            : Annotations(t)...
        {
        }
    };

    template <typename R, typename... Args>
	struct ConstructorImpl {
        template <typename T>
        ConstructorImpl(type_holder_t<T>)
            : mCtor([](Args &&...args) -> R{
                return std::make_unique<T>(std::forward<Args>(args)...);
            })
        {
        }

        R construct(Args&&... args) const
        {
            return mCtor(std::forward<Args>(args)...);
        }

        R (*mCtor)(Args&&...);
    };

    template <typename... Args>
    using Constructor = ConstructorImpl<Placeholder<0>, Args...>;


    template <typename R, typename... Args>
	struct FactoryImpl {
        template <typename T>
        FactoryImpl(type_holder_t<T>)
            : mFactory(T::factory)
        {
        }

        R create(Args&&... args) const
        {
            return mFactory(std::forward<Args>(args)...);
        }

        R (*mFactory)(Args&&...);
    };

    template <typename... Args>
    using Factory = FactoryImpl<Placeholder<0>, Args...>;

}
}