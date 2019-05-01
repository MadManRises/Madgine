#include "toolslib.h"
#include "clientlib.h"
#include "OpenGL/opengllib.h"
#include "oislib.h"
#include "Madgine/app/globalapicollector.h"
#include "renderer/imguiroot.h"
#include "gui/guisystem.h"
#include "input/inputcollector.h"
#include "oisinputhandler.h"
#include "render/renderercollector.h"
#include "OpenGL/openglrenderer.h"
#include "Madgine/resources/resourceloadercollector.h"
#include "OpenGL/openglmeshloader.h"
#include "OpenGL/openglshaderloader.h"
#include "inspector/layoutloader.h"
#include "toolscollector.h"
#include "inspector/inspector.h"
#include "metrics/metrics.h"
#include "profiler/profiler.h"
#include "ui/gamehandler.h"
#include "ui/guihandler.h"
#include "uniquecomponent/uniquecomponentshared.h"
#include "uniquecomponent/libA.cpp"
#include "uniquecomponent/libB.cpp"


namespace Engine{

	template <> std::vector<Engine::App::GlobalAPICollector::Registry::F> Engine::App::GlobalAPICollector::Registry::sComponents() { return {
		createComponent<Engine::Tools::ImGuiRoot>,
		createComponent<Engine::GUI::GUISystem>,

	}; }

    template<> size_t component_index<Engine::Tools::ImGuiRoot>(){ return 0; }
    template<> size_t component_index<Engine::GUI::GUISystem>(){ return 1; }

	template <> std::vector<Engine::Input::InputHandlerCollector::Registry::F> Engine::Input::InputHandlerCollector::Registry::sComponents() { return {
		createComponent<Engine::Input::OISInputHandler>,

	}; }

    template<> size_t component_index<Engine::Input::OISInputHandler>(){ return 0; }

	template <> std::vector<Engine::Render::RendererCollector::Registry::F> Engine::Render::RendererCollector::Registry::sComponents() { return {
		createComponent<Engine::Render::OpenGLRenderer>,

	}; }

    template<> size_t component_index<Engine::Render::OpenGLRenderer>(){ return 0; }

	template <> std::vector<Engine::Resources::ResourceLoaderCollector::Registry::F> Engine::Resources::ResourceLoaderCollector::Registry::sComponents() { return {
		createComponent<Engine::Render::OpenGLMeshLoader>,
		createComponent<Engine::Render::OpenGLShaderLoader>,
		createComponent<Engine::Tools::LayoutLoader>,

	}; }

    template<> size_t component_index<Engine::Render::OpenGLMeshLoader>(){ return 0; }
    template<> size_t component_index<Engine::Render::OpenGLShaderLoader>(){ return 1; }
    template<> size_t component_index<Engine::Tools::LayoutLoader>(){ return 2; }

	template <> std::vector<Engine::Tools::ToolsCollector::Registry::F> Engine::Tools::ToolsCollector::Registry::sComponents() { return {
		createComponent<Engine::Tools::Inspector>,
		createComponent<Engine::Tools::Metrics>,
		createComponent<Engine::Tools::Profiler>,

	}; }

    template<> size_t component_index<Engine::Tools::Inspector>(){ return 0; }
    template<> size_t component_index<Engine::Tools::Metrics>(){ return 1; }
    template<> size_t component_index<Engine::Tools::Profiler>(){ return 2; }

	template <> std::vector<Engine::UI::GameHandlerCollector::Registry::F> Engine::UI::GameHandlerCollector::Registry::sComponents() { return {

	}; }


	template <> std::vector<Engine::UI::GuiHandlerCollector::Registry::F> Engine::UI::GuiHandlerCollector::Registry::sComponents() { return {

	}; }


	template <> std::vector<Test::TestCollector::Registry::F> Test::TestCollector::Registry::sComponents() { return {
		createComponent<LibAComponent>,
		createComponent<LibBComponent>,

	}; }

    template<> size_t component_index<LibAComponent>(){ return 0; }
    template<> size_t component_index<LibBComponent>(){ return 1; }

}
