#pragma once

#include "../../reflection/classname.h"
#include "../../reflection/decay.h"

namespace Engine {

namespace Serialize {
    template <class T>
    class NoParentUnit : public T {
    public:
        template <class... Args>
        NoParentUnit(Args &&... args)
            : T(std::forward<Args>(args)...)
        {
            this->sync();
        }

        ~NoParentUnit()
        {
            this->unsync();
        }
    };
}

template <class T>
struct AccessClassInfo<Serialize::NoParentUnit<T>> {
    static inline const TypeInfo &get()
    {
        static const TypeInfo &tType = typeInfo<T>();
        static std::string className = "Engine::Serialize::NoParentUnit<"s + tType.mFullName + ">";
        static TypeInfo sInfo { className.c_str(), tType.mHeaderPath, &tType };
        return sInfo;
    }
};

template <typename T>
struct decay<Serialize::NoParentUnit<T>> {
	using type = T;
};

}
