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
            assert(!Base::_preg());
            Base::_preg() = &reg;
        }
        ~Inner()
        {
            assert(Base::_preg() == &reg);
            Base::_preg() = nullptr;
        }

    private:
        typename Base::Collector::template ComponentRegistrator<T> reg;
    };
};

DLL_IMPORT_VARIABLE2(typename T::Inner, _vreg, typename T, typename Base);

#    define VIRTUALUNIQUECOMPONENT(Name) DLL_EXPORT_VARIABLE2(, Name::Inner, Engine::, _vreg, {}, Name, Name::VBase)

template <typename _T, typename _Collector, typename Base>
struct VirtualUniqueComponentBase : public Base {
public:
    using Collector = _Collector;
    using T = _T;

    using Base::Base;

    static size_t component_index()
    {
        return _preg()->index();
    }

protected:
    static IndexHolder *&_preg()
    {
        static IndexHolder *dummy = nullptr;
        //LOG("Accessing registry for " << typeid(T).name() << " at: " << &dummy << "(= " << dummy << ")");
        return dummy;
    }
};

DLL_IMPORT_VARIABLE2(Engine::IndexHolder, _reg, typename T);

#    define UNIQUECOMPONENT_EX(Frontend, Full) DLL_EXPORT_VARIABLE3(, Engine::IndexHolder, Frontend::Collector::ComponentRegistrator<Full>, Engine::, _reg, , {}, Frontend)
#    define UNIQUECOMPONENT2(Name, ext) DLL_EXPORT_VARIABLE3(, Engine::IndexHolder, Name::Collector::ComponentRegistrator<Name>, Engine::, _reg, ext, {}, Name)

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

#    define UNIQUECOMPONENT_EX(Frontend, Full)
#    define UNIQUECOMPONENT2(Name, ext)
#    define VIRTUALUNIQUECOMPONENT(Name)

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

#define UNIQUECOMPONENT(Name) \
    UNIQUECOMPONENT_EX(Name, Name)

#define COMPONENT_NAME(Name, FrontendType) \
    DLL_EXPORT_VARIABLE2(constexpr, const std::string_view, Engine::, _componentName, #Name, FrontendType);

#define NAMED_UNIQUECOMPONENT_EX(Name, FrontendType, FullType) \
    COMPONENT_NAME(Name, FrontendType)                         \
    UNIQUECOMPONENT_EX(FrontendType, FullType)

#define NAMED_UNIQUECOMPONENT(Name, FullType) \
    NAMED_UNIQUECOMPONENT_EX(Name, FullType, FullType)