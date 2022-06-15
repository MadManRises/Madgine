#pragma once

#if ENABLE_PLUGINS

#    include "indexholder.h"

namespace Engine {

template <typename _T, typename Base, typename _VBase>
struct VirtualUniqueComponentImpl : Base {
    using VBase = _VBase;
    using T = _T;
    using Base::Base;

    struct Inner {
        Inner()
        {
            if (!Base::preg())
                Base::preg() = &reg;
        }
        ~Inner()
        {
            if (Base::preg() == &reg)
                Base::preg() = nullptr;
        }

    private:
        typename Base::Collector::template ComponentRegistrator<T> reg;
    };
};

DLL_IMPORT_VARIABLE2(typename T::Inner, _vreg, typename T, typename Base);

#    define VIRTUALUNIQUECOMPONENT(Name) DLL_EXPORT_VARIABLE2(, Name::Inner, Engine::, _vreg, {}, Name, Name::VBase)

DLL_IMPORT_VARIABLE2(Engine::IndexHolder *, _preg, typename T);

template <typename _T, typename _Collector, typename Base>
struct VirtualUniqueComponentBase : public Base {
public:
    using Collector = _Collector;
    using T = _T;

    using Base::Base;

    static size_t component_index()
    {
        return preg()->index();
    }

protected:
    static IndexHolder *&preg()
    {
        return _preg<T>();
    }
};

DLL_IMPORT_VARIABLE2(Engine::IndexHolder, _reg, typename T);

#    define UNIQUECOMPONENT(Type) DLL_EXPORT_VARIABLE3(, Engine::IndexHolder, Type::Collector::ComponentRegistrator<Type>, Engine::, _reg, , {}, Type)
#    define UNIQUECOMPONENT2(Type, ext) DLL_EXPORT_VARIABLE3(, Engine::IndexHolder, Type::Collector::ComponentRegistrator<Type>, Engine::, _reg, ext, {}, Type)

#    define VIRTUALUNIQUECOMPONENTBASE(Name) DLL_EXPORT_VARIABLE2(, Engine::IndexHolder *, Engine::, _preg, nullptr, Name)

template <typename _T, typename _Collector, typename _Base>
struct UniqueComponent : _Base {
    using Collector = _Collector;
    using T = _T;

    using _Base::_Base;

    static size_t component_index()
    {
        return _reg<T>().index();
    }
};

template <typename T>
size_t component_index()
{
    return T::component_index();
}

}

#else

namespace Engine {
template <typename _T, typename Base, typename _VBase>
struct VirtualUniqueComponentImpl : Base {
    using Base::Base;

    using T = _T;
};

template <typename _T, typename Collector, typename Base>
struct VirtualUniqueComponentBase : Base {
    using Base::Base;

    using T = _T;
};

template <typename _T, typename Collector, typename Base>
struct UniqueComponent : Base {
    using Base::Base;

    using T = _T;
};

template <typename T>
size_t component_index();

}

#    define UNIQUECOMPONENT(Type)
#    define UNIQUECOMPONENT2(Name, ext)
#    define VIRTUALUNIQUECOMPONENT(Name)
#    define VIRTUALUNIQUECOMPONENTBASE(Name)

#endif

namespace Engine {

DLL_IMPORT_VARIABLE2(const std::string_view, _componentName, typename T);

template <typename Base>
struct NamedUniqueComponent : Base {
    using Base::Base;

    static std::string_view componentName()
    {
        return _componentName<typename Base::T>();
    }
};

}

#define COMPONENT_NAME(Name, FrontendType) \
    DLL_EXPORT_VARIABLE2(constexpr, const std::string_view, Engine::, _componentName, #Name, FrontendType);

#define NAMED_UNIQUECOMPONENT(Name, Type) \
    COMPONENT_NAME(Name, Type)            \
    UNIQUECOMPONENT(Type)
