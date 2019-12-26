#pragma once

#include "serializablecontainer.h"

namespace Engine {
namespace Serialize {

    template <typename C, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    struct ControlledContainer : SerializableContainerImpl<C, Observer, OffsetPtr> {

		typedef SerializableContainerImpl<C, Observer, OffsetPtr> Base;

        struct traits : Base::traits {
            typedef ControlledContainer<C, Observer, OffsetPtr> container;
        };

        void writeState(Serialize::SerializeOutStream &out, const char *name) const
        {
            if (name)
                out.format().beginExtendedCompound(out, name);
            out.write(this->size(), "size");
            if (name)
                out.format().beginCompound(out, name);
            for (const auto &t : *this) {
                out.format().beginExtendedCompound(out, "Item");
                out.write(kvKey(t), "key");
                out.write(t, "Item");
            }
            if (name)
                out.format().endCompound(out, name);
        }

        void readState(Serialize::SerializeInStream &in, const char *name)
        {
            bool wasActive = this->beforeReset(true);

            if (name)
                in.format().beginExtendedCompound(in, name);
            decltype(this->size()) size;
            in.read(size, "size");
            if (name)
                in.format().beginCompound(in, name);

            auto physical = this->physical();

            std::remove_const_t<std::remove_reference_t<decltype(kvKey(*this->begin()))>> key;
            while (size--) {
                this->beginExtendedItem(in, *reinterpret_cast<const typename Base::type *>(0x1));
                in.read(key, "key");
                auto it = kvFind(physical, key);
                if (it != physical.end()) {
                    in.read(*it, "Item");
                } else {
                    LOG_ERROR("Could not find \"" << key << "\"!");
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
