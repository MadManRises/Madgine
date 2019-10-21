#pragma once

#include "serializablecontainer.h"

namespace Engine {
namespace Serialize {

    template <typename OffsetPtr, typename C>
    struct ControlledContainer : public SerializableContainerImpl<OffsetPtr, C> {
        struct traits : SerializableContainerImpl<OffsetPtr, C>::traits {
            typedef ControlledContainer<OffsetPtr, C> container;
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
            if (name)
                in.format().beginExtendedCompound(in, name);
            decltype(this->size()) size;
            in.read(size, "size");
            if (name)
                in.format().beginCompound(in, name);

            std::remove_const_t<std::remove_reference_t<decltype(kvKey(*this->begin()))>> key;
            while (size--) {
                in.format().beginExtendedCompound(in, "Item");
                in.read(key, "key");
                auto it = kvFind(*this, key);
                if (it != this->end()) {
                    in.read(*it, "Item");
                } else {
                    LOG_ERROR("Could not find \"" << key << "\"!");
					std::terminate();
				}
            }

            if (name)
                in.format().endCompound(in, name);
        }
    };

}

}
