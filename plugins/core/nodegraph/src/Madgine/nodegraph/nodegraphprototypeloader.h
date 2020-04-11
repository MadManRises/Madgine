#pragma once

#include "Modules/resources/resourceloader.h"

#include "nodegraphprototype.h"

#include "Modules/serialize/container/noparent.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeGraphPrototypeLoader : Resources::ResourceLoader<NodeGraphPrototypeLoader, Serialize::NoParentUnit<NodeGraphPrototype>, std::list<Placeholder<0>>> {

        using Base = Resources::ResourceLoader<NodeGraphPrototypeLoader, NodeGraphPrototype, std::list<Placeholder<0>>>;

        struct MADGINE_NODEGRAPH_EXPORT HandleType : Base::HandleType {
            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(handle)
            {
            }

            void create(const Filesystem::Path &path);
        };

        NodeGraphPrototypeLoader();

        bool loadImpl(Serialize::NoParentUnit<NodeGraphPrototype> &graph, ResourceType *res);
        void unloadImpl(Serialize::NoParentUnit<NodeGraphPrototype> &graph, ResourceType *res);

    };

}
}