#pragma once

namespace Engine
{
	namespace Tools
	{

		class MADGINE_TOOLS_EXPORT InspectorLayout
		{
		public:
			InspectorLayout(std::shared_ptr<tinyxml2::XMLDocument> &&document);

			tinyxml2::XMLElement *rootElement();

			std::vector<std::string> associations();


		private:

			std::shared_ptr<tinyxml2::XMLDocument> mDocument;
			
		};


	}
}
