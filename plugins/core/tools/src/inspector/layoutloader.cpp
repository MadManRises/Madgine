#include "../toolslib.h"

#include "layoutloader.h"

#include "../tinyxml/tinyxml2.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

UNIQUECOMPONENT(Engine::Tools::LayoutLoader);

namespace Engine {
namespace Tools {

    LayoutLoader::LayoutLoader()
        : ResourceLoader({ ".xml" })
    {
    }

    bool LayoutLoader::loadImpl(tinyxml2::XMLDocument &doc, ResourceType *res)
    {
        std::string content = res->readAsText();

        return !doc.Parse(content.c_str());
    }

    void LayoutLoader::unloadImpl(tinyxml2::XMLDocument &doc)
    {
        doc.Clear();
    }

}
}

METATABLE_BEGIN(Engine::Tools::LayoutLoader)
METATABLE_END(Engine::Tools::LayoutLoader)

METATABLE_BEGIN_BASE(Engine::Tools::LayoutLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Tools::LayoutLoader::ResourceType)

RegisterType(Engine::Tools::LayoutLoader);