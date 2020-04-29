#pragma once

#include "serializablecontainer.h"

#include "../../generic/comparator_traits.h"

#include "../../generic/fixstring.h"

namespace Engine {
namespace Serialize {

    template <typename C, typename Observer = NoOpFunctor, typename Cmp = typename container_traits<C>::cmp_type, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    struct ControlledContainer : SerializableContainerImpl<C, Observer, OffsetPtr> {

		typedef SerializableContainerImpl<C, Observer, OffsetPtr> Base;

        struct traits : Base::traits {
            typedef ControlledContainer<C, Observer, OffsetPtr> container;
            typedef Cmp cmp_type;
        };

        void writeState(Serialize::SerializeOutStream &out, const char *name) const
        {
            if (name)
                out.format().beginExtended(out, name);
            write(out, this->size(), "size");
            if (name)
                out.format().beginCompound(out, name);
            for (const auto &t : *this) {          
                this->beginExtendedItem(out, t);
                write(out, comparator_traits<Cmp>::to_cmp_type(t), "key");
                write(out, t, "Item");
            }
            if (name)
                out.format().endCompound(out, name);
        }

        void readState(Serialize::SerializeInStream &in, const char *name)
        {
            bool wasActive = this->beforeReset(true);

            if (name)
                in.format().beginExtended(in, name);
            decltype(this->size()) size;
            read(in, size, "size");
            if (name)
                in.format().beginCompound(in, name);

            FixString_t<typename comparator_traits<Cmp>::type> key;
            while (size--) {
                this->beginExtendedItem(in, nullref<const typename Base::value_type>);
                read(in, key, "key");
                auto it = std::find_if(this->physical().begin(), this->physical().end(), [&](const auto &t) { return comparator_traits<Cmp>::to_cmp_type(t) == key; });
                if (it != this->physical().end()) {
                    read(in, *it, "Item");
                } else {
                    LOG_ERROR("Could not find '" << key << "'!");
                    std::terminate();
                }
            }

            if (name)
                in.format().endCompound(in, name);

			this->afterReset(wasActive, true);
        }

        void setActive(bool active, bool existenceChange)
        {
            Base::setActive(active, existenceChange, true);
        }
    };

}

}
