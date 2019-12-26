#pragma once

#include "layoutloader.h"

namespace Engine
{
	namespace Tools
	{

		class MADGINE_TOOLS_EXPORT InspectorLayout
		{
		public:
			InspectorLayout(LayoutLoader::HandleType &&document);

			tinyxml2::XMLElement *rootElement();

			std::vector<std::string> associations();


		private:

			LayoutLoader::HandleType mDocument;
			
		};


	}
}
