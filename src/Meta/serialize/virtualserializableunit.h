#pragma once

#include "serializableunit.h"

#include "serializableunitptr.h"

namespace Engine {
namespace Serialize {

    template <typename Base = SerializableUnitBase, typename... Bases>
    struct DLL_EXPORT VirtualSerializableUnitBase : Base, Bases... {

        using Base::Base;

        virtual SerializableUnitPtr customUnitPtr() = 0;
        virtual SerializableUnitConstPtr customUnitPtr() const = 0;
    };

    template <typename T, typename _Base = VirtualSerializableUnitBase<>>
    struct VirtualUnit : _Base {
        using _Base::_Base;
        virtual ~VirtualUnit() = default;
        virtual SerializableUnitPtr customUnitPtr() override
        {
            return { this, &serializeTable<decayed_t<T>>() };
        }
        virtual SerializableUnitConstPtr customUnitPtr() const override
        {
            return { this, &serializeTable<decayed_t<T>>() };
        }
    };

    template <typename Base = SerializableDataUnit, typename... Bases>
    struct DLL_EXPORT VirtualSerializableDataBase : Base, Bases... {

        using Base::Base;

        virtual SerializableDataPtr customUnitPtr() = 0;
        virtual SerializableDataConstPtr customUnitPtr() const = 0;
    };

    template <typename T, typename _Base = VirtualSerializableDataBase<>>
    struct VirtualData : _Base {
        using _Base::_Base;
        virtual ~VirtualData() = default;
        virtual SerializableDataPtr customUnitPtr() override
        {
            return { this, &serializeTable<decayed_t<T>>() };
        }
        virtual SerializableDataConstPtr customUnitPtr() const override
        {
            return { this, &serializeTable<decayed_t<T>>() };
        }
    };

} // namespace Serialize
} // namespace Core
