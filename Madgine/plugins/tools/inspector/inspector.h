#pragma once
#include "../tool.h"
#include "../tinyxml/tinyxml2.h"

namespace Engine
{
	namespace Tools
	{
		
		class InspectorLayout
		{
		public:
			InspectorLayout(Inspector &inspector, const std::experimental::filesystem::path &path);

			void draw(Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawElement(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);

			std::vector<std::string> associations();

		protected:
			void drawSingleElement(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawElementList(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void inheritLayout(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawConstantString(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawSingleLine(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);

		private:
			std::experimental::filesystem::path mPath;

			tinyxml2::XMLDocument mDocument;

			static std::map<std::string, void (InspectorLayout::*)(tinyxml2::XMLElement *, Scripting::ScopeBase *, std::set<std::string>&)> sElements;

			Inspector &mInspector;
		};

		class Inspector : public Tool<Inspector>
		{
		public:
			Inspector(ImGuiRoot &root);

			virtual void render() override;

			void drawRemainingMembers(Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawValue(tinyxml2::XMLElement *element, Scripting::ScopeBase *parent, const std::unique_ptr<KeyValueIterator> &it);

			InspectorLayout * getLayout(const std::string& name);

			const char* key() override;

		protected:
			void draw(Scripting::ScopeBase *scope, const char *layoutName);
			
		private:
			std::map<std::string, InspectorLayout*> mAssociations;
			std::map<std::string, InspectorLayout> mLayouts;
		};

	}
}
