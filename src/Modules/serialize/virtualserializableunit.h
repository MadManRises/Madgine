#pragma once

#include "serializableunit.h"

#include "serializableunitptr.h"

namespace Engine {
namespace Serialize {

    template <typename Base = SerializableUnitBase>
    struct VirtualSerializableUnitBase : Base {

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

} // namespace Serialize
} // namespace Core
