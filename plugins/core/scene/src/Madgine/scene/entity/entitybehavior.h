#pragma once

#include "Meta/serialize/hierarchy/serializabledataunit.h"

#include "Madgine/nodegraph/nodegraphloader.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT EntityBehavior : Serialize::SerializableDataUnit {

            EntityBehavior(NodeGraph::NodeGraphLoader::Resource *graph, Entity *entity);
            EntityBehavior(std::string_view name, Entity *entity);
            EntityBehavior(EntityBehavior &&) = default;

            EntityBehavior &operator=(EntityBehavior &&) = default;

            std::string_view getName() const;

            NodeGraph::NodeGraphLoader::Resource *get() const;

            std::map<std::string_view, ValueType> variables();

            NodeGraph::NodeGraphLoader::Handle mGraph;
            NodeGraph::NodeInterpreter mState;
        };

    }
}
}