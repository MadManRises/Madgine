#pragma once

#include "../../generic/align.h"

namespace Engine {
namespace Serialize {

    struct Dummy {
    };

    template <auto P, typename Parent, size_t alignment>
    struct ObservableDummyOffset {
        template <typename Target, typename _Parent = Parent>
        static size_t offset()
        {

            size_t offset = reinterpret_cast<size_t>(&(static_cast<Parent *>(static_cast<_Parent *>(nullptr))->*P));

//#if !defined(__has_cpp_attribute) || !__has_cpp_attribute(no_unique_address)           
            offset = alignTo(offset + sizeof(Dummy), alignment);
//#endif

            ObservableBase *o = reinterpret_cast<ObservableBase *>(static_cast<char *>(nullptr) + offset);

            Target *t = static_cast<Target *>(o);

            size_t diff = reinterpret_cast<size_t>(t);

            return diff;
        }

        template <typename Target>
        static const Parent *parent(Target *p)
        {

            return reinterpret_cast<const Parent *>(reinterpret_cast<const char *>(p) - offset<Target>());
        }

    };

}
}