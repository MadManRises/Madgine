#pragma once

#if ENABLE_PLUGINS

#    include "../reflection/decay.h"
#    include "indexholder.h"

namespace Engine {

template <typename T, typename _Base>
struct VirtualUniqueComponentImpl : _Base {
    using Base = _Base;
    using Base::Base;

    //typedef Base decay_t;

    struct Inner {
        Inner()
        {
            Base::_preg() = &reg;
        }

    private:
        typename Base::Collector::template ComponentRegistrator<T> reg;
    };
};

DLL_IMPORT_VARIABLE2(typename T::Inner, _vreg, typename T, typename Base);

#    define VIRTUALUNIQUECOMPONENT(Name) DLL_EXPORT_VARIABLE2(, Name::Inner, Engine::, _vreg, {}, Name, Name::Base)

template <class T, class _Collector, typename Base = typename _Collector::Base>
class VirtualUniqueComponentBase : public Base {
public:
    using Collector = _Collector;

    using Base::Base;

    static size_t component_index()
    {
        return _preg()->index();
    }

protected:
    static IndexHolder *&_preg()
    {
        static IndexHolder *dummy;
        return dummy;
    }
};

DLL_IMPORT_VARIABLE2(typename Collector::template ComponentRegistrator<T>, _reg, typename T, typename Collector);

#    define UNIQUECOMPONENT(Name) DLL_EXPORT_VARIABLE2(, Name::Collector::ComponentRegistrator<Name>, Engine::, _reg, {}, Name, Name::Collector)

template <typename T, typename _Collector, typename Base = typename _Collector::Base>
struct UniqueComponent : Base {
    using Collector = _Collector;

    using Base::Base;

    static size_t component_index()
    {
        return _reg<T, Collector>().index();
    }
};

template <class T>
size_t component_index()
{
    return T::component_index();
}
}

#else

namespace Engine {
template <class T, class Base>
struct VirtualUniqueComponentImpl : Base {
	using Base::Base;
};

template <class T, class Collector, typename Base = typename Collector::Base>
struct VirtualUniqueComponentBase : Base {
    using Base::Base;
};

template <class T, class Collector, typename Base = typename Collector::Base>
struct UniqueComponent : Base {
    using Base::Base;
};

template <class T>
size_t component_index();
}

#    define UNIQUECOMPONENT(Name)
#    define VIRTUALUNIQUECOMPONENT(Name)

#endif