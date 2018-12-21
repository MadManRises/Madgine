#pragma once
#include "../tool.h"

#include "inspectorlayout.h"

namespace Engine
{
	namespace Tools
	{
		

		class MADGINE_TOOLS_EXPORT Inspector : public Tool<Inspector>
		{
		public:
			Inspector(ImGuiRoot &root);
			Inspector(const Inspector &) = delete;
			~Inspector();

			virtual void render() override;

			void drawRemainingMembers(Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawValue(tinyxml2::XMLElement *element, Scripting::ScopeBase *parent, const KeyValueIterator &it);

			InspectorLayout * getLayout(const std::string& name);

			const char* key() override;

		private:
			void draw(Scripting::ScopeBase *scope, const char *layoutName);

			void draw(InspectorLayout *layout, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawElement(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);

			void drawSingleElement(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawElementList(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void inheritLayout(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawConstantString(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			void drawSingleLine(tinyxml2::XMLElement *element, Scripting::ScopeBase *scope, std::set<std::string> &drawn);
			
		private:
			std::map<std::string, InspectorLayout*> mAssociations;
			std::map<std::string, InspectorLayout> mLayouts;

			static std::map<std::string, void (Inspector::*)(tinyxml2::XMLElement *, Scripting::ScopeBase *, std::set<std::string>&)> sElements;
		};

	}
}

RegisterType(Engine::Tools::Inspector);
