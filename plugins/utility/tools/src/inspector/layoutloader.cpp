#include "../toolslib.h"

#include "layoutloader.h"

#include "../tinyxml/tinyxml2.h"

#include "Modules/reflection/classname.h"
#include "Modules/keyvalue/metatable_impl.h"

UNIQUECOMPONENT(Engine::Tools::LayoutLoader);

namespace Engine {
	namespace Tools {
		
		LayoutLoader::LayoutLoader(Resources::ResourceManager & mgr) :
			ResourceLoader(mgr, { ".xml" })
		{
		}

		std::shared_ptr<tinyxml2::XMLDocument> Engine::Tools::LayoutLoader::loadImpl(ResourceType * res)
		{
			std::shared_ptr<tinyxml2::XMLDocument> doc = std::make_shared<tinyxml2::XMLDocument>();
			if (doc->LoadFile(res->path().c_str()))
				return {};
			return doc;
		}

	}
}

METATABLE_BEGIN(Engine::Tools::LayoutLoader)
METATABLE_END(Engine::Tools::LayoutLoader)

RegisterType(Engine::Tools::LayoutLoader);