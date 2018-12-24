#ifdef BUILD_PLUGIN_OISHandler
#include "oislib.h"
#endif
#ifdef BUILD_PLUGIN_Tools
#include "toolslib.h"
#endif
#ifdef BUILD_Base
#include "Madgine/baselib.h"
#endif
#ifdef BUILD_Client
#include "Madgine/clientlib.h"
#endif
#ifdef BUILD_PLUGIN_OpenGL
#include "OpenGL/opengllib.h"
#endif
#ifdef BUILD_Base
#include "Madgine/app/globalapicomponentcollector.h"
#endif
#ifdef BUILD_Base
#include "Madgine/scene/scenemanager.h"
#endif
#ifdef BUILD_PLUGIN_Tools
#include "renderer/imguiroot.h"
#endif
#ifdef BUILD_Client
#include "Madgine/input/inputcollector.h"
#endif
#ifdef BUILD_PLUGIN_OISHandler
#include "oisinputhandler.h"
#endif
#ifdef BUILD_Client
#include "Madgine/render/renderer.h"
#endif
#ifdef BUILD_PLUGIN_OpenGL
#include "OpenGL/openglrenderer.h"
#endif
#ifdef BUILD_Base
#include "Madgine/resources/resourceloadercollector.h"
#endif
#ifdef BUILD_Base
#include "Madgine/resources/scripts/scriptloader.h"
#endif
#ifdef BUILD_PLUGIN_OpenGL
#include "OpenGL/openglshaderloader.h"
#endif
#ifdef BUILD_PLUGIN_Tools
#include "inspector/layoutloader.h"
#endif
#ifdef BUILD_Base
#include "Madgine/scene/scenecomponentcollector.h"
#endif
#ifdef BUILD_PLUGIN_Tools
#include "toolscollector.h"
#endif
#ifdef BUILD_PLUGIN_Tools
#include "inspector/inspector.h"
#endif
#ifdef BUILD_PLUGIN_Tools
#include "metrics/metrics.h"
#endif
#ifdef BUILD_PLUGIN_Tools
#include "profiler/profiler.h"
#endif
#ifdef BUILD_PLUGIN_Tools
#include "renderer/imguidemo.h"
#endif
#ifdef BUILD_Client
#include "Madgine/ui/gamehandler.h"
#endif
#ifdef BUILD_Client
#include "Madgine/ui/guihandler.h"
#endif


namespace Engine {

#ifdef BUILD_Base
	template<> std::vector<Engine::App::GlobalAPICollector::Registry::F> Engine::App::GlobalAPICollector::Registry::sComponents() {
		return {
#ifdef BUILD_Base
		createComponent<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>,
#endif
#ifdef BUILD_PLUGIN_Tools
		createComponent<Engine::Tools::ImGuiRoot>,
#endif

		};
	}

#endif
#ifdef BUILD_Base
	template<> size_t component_index<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>() { return -1; }
	template<> size_t component_index<Engine::Scene::SceneManager>() { return -1; }
#endif
#ifdef BUILD_PLUGIN_Tools
	template<> size_t component_index<Engine::Tools::ImGuiRoot>() { return -1; }
#endif

#ifdef BUILD_Client
	template<> std::vector<Engine::Input::InputCollector::Registry::F> Engine::Input::InputCollector::Registry::sComponents() {
		return {
#ifdef BUILD_PLUGIN_OISHandler
		createComponent<Engine::Input::OISInputHandler>,
#endif

		};
	}

#endif
#ifdef BUILD_PLUGIN_OISHandler
	template<> size_t component_index<Engine::Input::OISInputHandler>() { return -1; }
#endif

#ifdef BUILD_Client
	template<> std::vector<Engine::Render::RendererCollector::Registry::F> Engine::Render::RendererCollector::Registry::sComponents() {
		return {
#ifdef BUILD_PLUGIN_OpenGL
		createComponent<Engine::Render::OpenGLRenderer>,
#endif

		};
	}

#endif
#ifdef BUILD_PLUGIN_OpenGL
	template<> size_t component_index<Engine::Render::OpenGLRenderer>() { return -1; }
#endif

#ifdef BUILD_Base
	template<> std::vector<Engine::Resources::ResourceLoaderCollector::Registry::F> Engine::Resources::ResourceLoaderCollector::Registry::sComponents() {
		return {
#ifdef BUILD_Base
		createComponent<Engine::Scripting::Parsing::ScriptLoader>,
#endif
#ifdef BUILD_PLUGIN_OpenGL
		createComponent<Engine::Render::OpenGLShaderLoader>,
#endif
#ifdef BUILD_PLUGIN_Tools
		createComponent<Engine::Tools::LayoutLoader>,
#endif

		};
	}

#endif
#ifdef BUILD_Base
	template<> size_t component_index<Engine::Scripting::Parsing::ScriptLoader>() { return -1; }
#endif
#ifdef BUILD_PLUGIN_OpenGL
	template<> size_t component_index<Engine::Render::OpenGLShaderLoader>() { return -1; }
#endif
#ifdef BUILD_PLUGIN_Tools
	template<> size_t component_index<Engine::Tools::LayoutLoader>() { return -1; }
#endif

#ifdef BUILD_Base
	template<> std::vector<Engine::Scene::SceneComponentCollector::Registry::F> Engine::Scene::SceneComponentCollector::Registry::sComponents() {
		return {

		};
	}

#endif

#ifdef BUILD_PLUGIN_Tools
	template<> std::vector<Engine::Tools::ToolsCollector::Registry::F> Engine::Tools::ToolsCollector::Registry::sComponents() {
		return {
#ifdef BUILD_PLUGIN_Tools
		createComponent<Engine::Tools::Inspector>,
#endif
#ifdef BUILD_PLUGIN_Tools
		createComponent<Engine::Tools::Metrics>,
#endif
#ifdef BUILD_PLUGIN_Tools
		createComponent<Engine::Tools::Profiler>,
#endif
#ifdef BUILD_PLUGIN_Tools
		createComponent<Engine::Tools::ImGuiDemo>,
#endif

		};
	}

#endif
#ifdef BUILD_PLUGIN_Tools
	template<> size_t component_index<Engine::Tools::Inspector>() { return -1; }
#endif
#ifdef BUILD_PLUGIN_Tools
	template<> size_t component_index<Engine::Tools::Metrics>() { return -1; }
#endif
#ifdef BUILD_PLUGIN_Tools
	template<> size_t component_index<Engine::Tools::Profiler>() { return -1; }
#endif
#ifdef BUILD_PLUGIN_Tools
	template<> size_t component_index<Engine::Tools::ImGuiDemo>() { return -1; }
#endif

#ifdef BUILD_Client
	template<> std::vector<Engine::UI::GameHandlerCollector::Registry::F> Engine::UI::GameHandlerCollector::Registry::sComponents() {
		return {

		};
	}

#endif

#ifdef BUILD_Client
	template<> std::vector<Engine::UI::GuiHandlerCollector::Registry::F> Engine::UI::GuiHandlerCollector::Registry::sComponents() {
		return {

		};
	}

#endif

}