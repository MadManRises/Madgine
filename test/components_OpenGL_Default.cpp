#ifdef BUILD_Base
#include "toolslib.h"
#include "Madgine/baselib.h"
#include "oislib.h"
#include "OpenGL/opengllib.h"
#include "Madgine/app/globalapicollector.h"
#include "Madgine/scene/scenemanager.h"
#include "renderer/imguiroot.h"
#include "Madgine/input/inputcollector.h"
#include "oisinputhandler.h"
#include "Madgine/render/renderercollector.h"
#include "OpenGL/openglrenderer.h"
#include "Madgine/resources/resourceloadercollector.h"
#include "Madgine/resources/scripts/scriptloader.h"
#include "OpenGL/openglshaderloader.h"
#include "inspector/layoutloader.h"
#include "Madgine/scene/scenecomponentcollector.h"
#include "toolscollector.h"
#include "inspector/inspector.h"
#include "metrics/metrics.h"
#include "profiler/profiler.h"
#include "renderer/imguidemo.h"
#include "Madgine/ui/gamehandler.h"
#include "Madgine/ui/guihandler.h"


namespace Engine {

	template<> std::vector<Engine::App::GlobalAPICollector::Registry::F> Engine::App::GlobalAPICollector::Registry::sComponents() {
		return {
createComponent<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>,
createComponent<Engine::Tools::ImGuiRoot>,

		};
	}

	template<> size_t component_index<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>() { return 0; }
	template<> size_t component_index<Engine::Scene::SceneManager>() { return 0; }
	template<> size_t component_index<Engine::Tools::ImGuiRoot>() { return 1; }

	template<> std::vector<Engine::Input::InputHandlerCollector::Registry::F> Engine::Input::InputHandlerCollector::Registry::sComponents() {
		return {
createComponent<Engine::Input::OISInputHandler>,

		};
	}

	template<> size_t component_index<Engine::Input::OISInputHandler>() { return 0; }

	template<> std::vector<Engine::Render::RendererCollector::Registry::F> Engine::Render::RendererCollector::Registry::sComponents() {
		return {
createComponent<Engine::Render::OpenGLRenderer>,

		};
	}

	template<> size_t component_index<Engine::Render::OpenGLRenderer>() { return 0; }

	template<> std::vector<Engine::Resources::ResourceLoaderCollector::Registry::F> Engine::Resources::ResourceLoaderCollector::Registry::sComponents() {
		return {
createComponent<Engine::Scripting::Parsing::ScriptLoader>,
createComponent<Engine::Render::OpenGLShaderLoader>,
createComponent<Engine::Tools::LayoutLoader>,

		};
	}

	template<> size_t component_index<Engine::Scripting::Parsing::ScriptLoader>() { return 0; }
	template<> size_t component_index<Engine::Render::OpenGLShaderLoader>() { return 1; }
	template<> size_t component_index<Engine::Tools::LayoutLoader>() { return 2; }

	template<> std::vector<Engine::Scene::SceneComponentCollector::Registry::F> Engine::Scene::SceneComponentCollector::Registry::sComponents() {
		return {

		};
	}


	template<> std::vector<Engine::Tools::ToolsCollector::Registry::F> Engine::Tools::ToolsCollector::Registry::sComponents() {
		return {
createComponent<Engine::Tools::Inspector>,
createComponent<Engine::Tools::Metrics>,
createComponent<Engine::Tools::Profiler>,
createComponent<Engine::Tools::ImGuiDemo>,

		};
	}

	template<> size_t component_index<Engine::Tools::Inspector>() { return 0; }
	template<> size_t component_index<Engine::Tools::Metrics>() { return 1; }
	template<> size_t component_index<Engine::Tools::Profiler>() { return 2; }
	template<> size_t component_index<Engine::Tools::ImGuiDemo>() { return 3; }

	template<> std::vector<Engine::UI::GameHandlerCollector::Registry::F> Engine::UI::GameHandlerCollector::Registry::sComponents() {
		return {

		};
	}


	template<> std::vector<Engine::UI::GuiHandlerCollector::Registry::F> Engine::UI::GuiHandlerCollector::Registry::sComponents() {
		return {

		};
	}


}
#endif
