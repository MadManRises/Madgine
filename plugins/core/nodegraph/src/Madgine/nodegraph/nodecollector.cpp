#include "../nodegraphlib.h"

#include "nodecollector.h"
#include "Modules/uniquecomponent/uniquecomponentregistry.h"

DEFINE_UNIQUE_COMPONENT(Engine::NodeGraph, Node)

namespace Engine {
namespace NodeGraph {

	std::map<std::string_view, IndexRef> &sNodesByName()
    {
        static std::map<std::string_view, IndexRef> dummy;
        return dummy;
    } 

}
}