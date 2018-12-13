#include "../toolslib.h"

#include "layoutloader.h"

#include "../tinyxml/tinyxml2.h"

namespace Engine {
	namespace Tools {
		
		LayoutLoader::LayoutLoader(Resources::ResourceManager & mgr) :
			ResourceLoader(mgr, { ".xml" })
		{
		}

		std::shared_ptr<tinyxml2::XMLDocument> Engine::Tools::LayoutLoader::load(ResourceType * res)
		{
			std::shared_ptr<tinyxml2::XMLDocument> doc = std::make_shared<tinyxml2::XMLDocument>();
			if (doc->LoadFile(res->path().generic_string().c_str()))
				return {};
			return doc;
		}

	}
}