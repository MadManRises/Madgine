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

    bool LayoutLoader::loadImpl(XMLDocument &doc, ResourceType *res)
    {
        std::string content = res->readAsText();

        return !doc->Parse(content.c_str());
    }

    void LayoutLoader::unloadImpl(XMLDocument &doc, ResourceType *res)
    {
        doc->Clear();
    }

	XMLDocument::XMLDocument()
        : mPtr(std::make_unique<tinyxml2::XMLDocument>())
    {
    }

    XMLDocument::~XMLDocument()        
    {
    }

    XMLDocument::XMLDocument(XMLDocument &&other)
        : mPtr(std::move(other.mPtr))
    {
    }

    XMLDocument &XMLDocument::operator=(XMLDocument &&other)
    {
        mPtr = std::move(other.mPtr);
        return *this;
    }

}
}

METATABLE_BEGIN(Engine::Tools::LayoutLoader)
METATABLE_END(Engine::Tools::LayoutLoader)

METATABLE_BEGIN_BASE(Engine::Tools::LayoutLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Tools::LayoutLoader::ResourceType)

