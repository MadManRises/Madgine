
#include "Madgine/baselib.h"

#include "toolslib.h"
#include "oislib.h"
#include "OpenGL/opengllib.h"

#include "../src/Madgine/app/globalapicomponentcollector.h"
#include "../src/Madgine/scene/scenemanager.h"
#include "../plugins/utility/tools/src/renderer/imguiroot.h"
#include "../src/Madgine/input/inputcollector.h"
#include "../plugins/input/ois/src/oisinputhandler.h"
#include "../src/Madgine/render/renderer.h"
#include "../plugins/renderer/opengl/src/OpenGL/openglrenderer.h"
#include "../src/Madgine/resources/resourceloadercollector.h"
#include "../src/Madgine/resources/scripts/scriptloader.h"
#include "../plugins/renderer/opengl/src/OpenGL/openglshaderloader.h"
#include "../plugins/utility/tools/src/inspector/layoutloader.h"
#include "../src/Madgine/scene/scenecomponentcollector.h"
#include "../plugins/utility/tools/src/toolscollector.h"
#include "../plugins/utility/tools/src/inspector/inspector.h"
#include "../plugins/utility/tools/src/memory/memoryviewer.h"
#include "../plugins/utility/tools/src/metrics/metrics.h"
#include "../plugins/utility/tools/src/profiler/profiler.h"
#include "../plugins/utility/tools/src/renderer/imguidemo.h"
#include "../src/Madgine/ui/gamehandler.h"
#include "../src/Madgine/ui/guihandler.h"


namespace Engine{

	template<> std::vector<Engine::App::GlobalAPICollector::Registry::F> Engine::App::GlobalAPICollector::Registry::sComponents() { return {
		createComponent<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>,
		createComponent<Engine::Tools::ImGuiRoot>
	}; }

    template<> size_t component_index<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>(){ return 0; }
    template<> size_t component_index<Engine::Scene::SceneManager>(){ return 0; }
    template<> size_t component_index<Engine::Tools::ImGuiRoot>(){ return 1; }

	template<> std::vector<Engine::Input::InputCollector::Registry::F> Engine::Input::InputCollector::Registry::sComponents() { return {
		createComponent<Engine::Input::OISInputHandler>
	}; }

    template<> size_t component_index<Engine::Input::OISInputHandler>(){ return 0; }

	template<> std::vector<Engine::Render::RendererCollector::Registry::F> Engine::Render::RendererCollector::Registry::sComponents() { return {
		createComponent<Engine::Render::OpenGLRenderer>
	}; }

    template<> size_t component_index<Engine::Render::OpenGLRenderer>(){ return 0; }

	template<> std::vector<Engine::Resources::ResourceLoaderCollector::Registry::F> Engine::Resources::ResourceLoaderCollector::Registry::sComponents() { return {
		createComponent<Engine::Scripting::Parsing::ScriptLoader>,
		createComponent<Engine::Render::OpenGLShaderLoader>,
		createComponent<Engine::Tools::LayoutLoader>
	}; }

    template<> size_t component_index<Engine::Scripting::Parsing::ScriptLoader>(){ return 0; }
    template<> size_t component_index<Engine::Render::OpenGLShaderLoader>(){ return 1; }
    template<> size_t component_index<Engine::Tools::LayoutLoader>(){ return 2; }

	template<> std::vector<Engine::Scene::SceneComponentCollector::Registry::F> Engine::Scene::SceneComponentCollector::Registry::sComponents() { return {

	}; }


	template<> std::vector<Engine::Tools::ToolsCollector::Registry::F> Engine::Tools::ToolsCollector::Registry::sComponents() { return {
		createComponent<Engine::Tools::Inspector>,
		createComponent<Engine::Tools::Metrics>,
		createComponent<Engine::Tools::Profiler>,
		createComponent<Engine::Tools::ImGuiDemo>
	}; }

    template<> size_t component_index<Engine::Tools::Inspector>(){ return 0; }    
    template<> size_t component_index<Engine::Tools::Metrics>(){ return 1; }
    template<> size_t component_index<Engine::Tools::Profiler>(){ return 2; }
    template<> size_t component_index<Engine::Tools::ImGuiDemo>(){ return 3; }

	template<> std::vector<Engine::UI::GameHandlerCollector::Registry::F> Engine::UI::GameHandlerCollector::Registry::sComponents() { return {

	}; }


	template<> std::vector<Engine::UI::GuiHandlerCollector::Registry::F> Engine::UI::GuiHandlerCollector::Registry::sComponents() { return {

	}; }


}