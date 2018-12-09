#include "../toolslib.h"

#include "inspectorlayout.h"

#include "../tinyxml/tinyxml2.h"

namespace Engine {
	namespace Tools {		

		InspectorLayout::InspectorLayout(std::shared_ptr<tinyxml2::XMLDocument> &&document) :
			mDocument(std::move(document))
		{			
		}

		tinyxml2::XMLElement * InspectorLayout::rootElement()
		{
			return mDocument->RootElement();
		}

		
		std::vector<std::string> InspectorLayout::associations()
		{
			std::vector<std::string> result;
			for (tinyxml2::XMLElement *el = mDocument->RootElement()->FirstChildElement("Association"); el; el = el->NextSiblingElement("Association"))
			{
				result.push_back(el->GetText());
			}
			return result;
		}



	}
}