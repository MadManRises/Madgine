#pragma once

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

    template <typename Base, typename... Args>
	struct Constructor {
        template <typename T>
        Constructor(type_holder_t<T>)
            : mCtor([](Args &&...args) -> std::unique_ptr<Base>{
                return std::make_unique<T>(std::forward<Args>(args)...);
            })
        {
        }

        std::unique_ptr<Base> construct(Args&&... args) const
        {
            return mCtor(std::forward<Args>(args)...);
        }

        std::unique_ptr<Base> (*mCtor)(Args&&...);
    };

}
}