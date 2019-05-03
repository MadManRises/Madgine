#include "Interfaces/interfaceslib.h"
#ifdef BUILD_Tools
#include "toolslib.h"
#endif
#ifdef BUILD_Client
#include "clientlib.h"
#endif
#ifdef BUILD_Base
#include "Madgine/baselib.h"
#endif
#ifdef BUILD_OpenGL
#include "OpenGL/opengllib.h"
#endif
#ifdef BUILD_OISHandler
#include "oislib.h"
#endif
#ifdef BUILD_Base
#include "Madgine/app/globalapicollector.h"
#endif
#ifdef BUILD_Tools
#include "renderer/imguiroot.h"
#endif
#ifdef BUILD_Base
#include "Madgine/scene/scenemanager.h"
#endif
#ifdef BUILD_Client
#include "gui/guisystem.h"
#endif
#ifdef BUILD_Client
#include "input/inputcollector.h"
#endif
#ifdef BUILD_OISHandler
#include "oisinputhandler.h"
#endif
#ifdef BUILD_Client
#include "render/renderercollector.h"
#endif
#ifdef BUILD_OpenGL
#include "OpenGL/openglrenderer.h"
#endif
#ifdef BUILD_Base
#include "Madgine/resources/resourceloadercollector.h"
#endif
#ifdef BUILD_OpenGL
#include "OpenGL/openglmeshloader.h"
#endif
#ifdef BUILD_OpenGL
#include "OpenGL/openglshaderloader.h"
#endif
#ifdef BUILD_Tools
#include "inspector/layoutloader.h"
#endif
#ifdef BUILD_Base
#include "Madgine/resources/scripts/scriptloader.h"
#endif
#ifdef BUILD_Base
#include "Madgine/scene/scenecomponentcollector.h"
#endif
#ifdef BUILD_Tools
#include "toolscollector.h"
#endif
#ifdef BUILD_Tools
#include "inspector/inspector.h"
#endif
#ifdef BUILD_Tools
#include "metrics/metrics.h"
#endif
#ifdef BUILD_Tools
#include "profiler/profiler.h"
#endif
#ifdef BUILD_Client
#include "ui/gamehandler.h"
#endif
#ifdef BUILD_Client
#include "ui/guihandler.h"
#endif
#ifdef BUILD_TestShared
#include "uniquecomponent/uniquecomponentshared.h"
#endif
#ifdef BUILD_LibA
#include "uniquecomponent/libA.cpp"
#endif
#ifdef BUILD_LibB
#include "uniquecomponent/libB.cpp"
#endif


namespace Engine{

#ifdef BUILD_Base
	template <> std::vector<Engine::App::GlobalAPICollector::Registry::F> Engine::App::GlobalAPICollector::Registry::sComponents() {
		return {
#ifdef BUILD_Tools
			createComponent<Engine::Tools::ImGuiRoot>,
#endif
#ifdef BUILD_Base
			createComponent<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>,
#endif
#ifdef BUILD_Client
			createComponent<Engine::GUI::GUISystem>,
#endif

		}; 
	}

	#define ACC 0

#ifdef BUILD_Tools
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Tools = ACC;
    template<> size_t component_index<Engine::Tools::ImGuiRoot>(){ return CollectorBaseIndex_GlobalAPIBase_Tools + 0; }
#undef ACC
#define ACC CollectorBaseIndex_GlobalAPIBase_Tools + 1
#endif
#ifdef BUILD_Base
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Base = ACC;
    template<> size_t component_index<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>(){ return CollectorBaseIndex_GlobalAPIBase_Base + 0; }
    template<> size_t component_index<Engine::Scene::SceneManager>(){ return CollectorBaseIndex_GlobalAPIBase_Base + 0; }
#undef ACC
#define ACC CollectorBaseIndex_GlobalAPIBase_Base + 1
#endif
#ifdef BUILD_Client
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Client = ACC;
    template<> size_t component_index<Engine::GUI::GUISystem>(){ return CollectorBaseIndex_GlobalAPIBase_Client + 0; }
#undef ACC
#define ACC CollectorBaseIndex_GlobalAPIBase_Client + 1
#endif

#undef ACC

#endif
#ifdef BUILD_Client
	template <> std::vector<Engine::Input::InputHandlerCollector::Registry::F> Engine::Input::InputHandlerCollector::Registry::sComponents() {
		return {
#ifdef BUILD_OISHandler
			createComponent<Engine::Input::OISInputHandler>,
#endif

		}; 
	}

	#define ACC 0

#ifdef BUILD_OISHandler
constexpr size_t CollectorBaseIndex_InputHandler_OISHandler = ACC;
    template<> size_t component_index<Engine::Input::OISInputHandler>(){ return CollectorBaseIndex_InputHandler_OISHandler + 0; }
#undef ACC
#define ACC CollectorBaseIndex_InputHandler_OISHandler + 1
#endif

#undef ACC

#endif
#ifdef BUILD_Client
	template <> std::vector<Engine::Render::RendererCollector::Registry::F> Engine::Render::RendererCollector::Registry::sComponents() {
		return {
#ifdef BUILD_OpenGL
			createComponent<Engine::Render::OpenGLRenderer>,
#endif

		}; 
	}

	#define ACC 0

#ifdef BUILD_OpenGL
constexpr size_t CollectorBaseIndex_RendererBase_OpenGL = ACC;
    template<> size_t component_index<Engine::Render::OpenGLRenderer>(){ return CollectorBaseIndex_RendererBase_OpenGL + 0; }
#undef ACC
#define ACC CollectorBaseIndex_RendererBase_OpenGL + 1
#endif

#undef ACC

#endif
#ifdef BUILD_Base
	template <> std::vector<Engine::Resources::ResourceLoaderCollector::Registry::F> Engine::Resources::ResourceLoaderCollector::Registry::sComponents() {
		return {
#ifdef BUILD_OpenGL
			createComponent<Engine::Render::OpenGLMeshLoader>,
			createComponent<Engine::Render::OpenGLShaderLoader>,
#endif
#ifdef BUILD_Tools
			createComponent<Engine::Tools::LayoutLoader>,
#endif
#ifdef BUILD_Base
			createComponent<Engine::Scripting::Parsing::ScriptLoader>,
#endif

		}; 
	}

	#define ACC 0

#ifdef BUILD_OpenGL
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_OpenGL = ACC;
    template<> size_t component_index<Engine::Render::OpenGLMeshLoader>(){ return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 0; }
    template<> size_t component_index<Engine::Render::OpenGLShaderLoader>(){ return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
#undef ACC
#define ACC CollectorBaseIndex_ResourceLoaderBase_OpenGL + 2
#endif
#ifdef BUILD_Tools
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_Tools = ACC;
    template<> size_t component_index<Engine::Tools::LayoutLoader>(){ return CollectorBaseIndex_ResourceLoaderBase_Tools + 0; }
#undef ACC
#define ACC CollectorBaseIndex_ResourceLoaderBase_Tools + 1
#endif
#ifdef BUILD_Base
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_Base = ACC;
    template<> size_t component_index<Engine::Scripting::Parsing::ScriptLoader>(){ return CollectorBaseIndex_ResourceLoaderBase_Base + 0; }
#undef ACC
#define ACC CollectorBaseIndex_ResourceLoaderBase_Base + 1
#endif

#undef ACC

#endif
#ifdef BUILD_Base
	template <> std::vector<Engine::Scene::SceneComponentCollector::Registry::F> Engine::Scene::SceneComponentCollector::Registry::sComponents() {
		return {

		}; 
	}

	#define ACC 0


#undef ACC

#endif
#ifdef BUILD_Tools
	template <> std::vector<Engine::Tools::ToolsCollector::Registry::F> Engine::Tools::ToolsCollector::Registry::sComponents() {
		return {
#ifdef BUILD_Tools
			createComponent<Engine::Tools::Inspector>,
			createComponent<Engine::Tools::Metrics>,
			createComponent<Engine::Tools::Profiler>,
#endif

		}; 
	}

	#define ACC 0

#ifdef BUILD_Tools
constexpr size_t CollectorBaseIndex_ToolBase_Tools = ACC;
    template<> size_t component_index<Engine::Tools::Inspector>(){ return CollectorBaseIndex_ToolBase_Tools + 0; }
    template<> size_t component_index<Engine::Tools::Metrics>(){ return CollectorBaseIndex_ToolBase_Tools + 1; }
    template<> size_t component_index<Engine::Tools::Profiler>(){ return CollectorBaseIndex_ToolBase_Tools + 2; }
#undef ACC
#define ACC CollectorBaseIndex_ToolBase_Tools + 3
#endif

#undef ACC

#endif
#ifdef BUILD_Client
	template <> std::vector<Engine::UI::GameHandlerCollector::Registry::F> Engine::UI::GameHandlerCollector::Registry::sComponents() {
		return {

		}; 
	}

	#define ACC 0


#undef ACC

#endif
#ifdef BUILD_Client
	template <> std::vector<Engine::UI::GuiHandlerCollector::Registry::F> Engine::UI::GuiHandlerCollector::Registry::sComponents() {
		return {

		}; 
	}

	#define ACC 0


#undef ACC

#endif
#ifdef BUILD_TestShared
	template <> std::vector<Test::TestCollector::Registry::F> Test::TestCollector::Registry::sComponents() {
		return {
#ifdef BUILD_LibA
			createComponent<LibAComponent>,
#endif
#ifdef BUILD_LibB
			createComponent<LibBComponent>,
#endif

		}; 
	}

	#define ACC 0

#ifdef BUILD_LibA
constexpr size_t CollectorBaseIndex_TestBase_LibA = ACC;
    template<> size_t component_index<LibAComponent>(){ return CollectorBaseIndex_TestBase_LibA + 0; }
#undef ACC
#define ACC CollectorBaseIndex_TestBase_LibA + 1
#endif
#ifdef BUILD_LibB
constexpr size_t CollectorBaseIndex_TestBase_LibB = ACC;
    template<> size_t component_index<LibBComponent>(){ return CollectorBaseIndex_TestBase_LibB + 0; }
#undef ACC
#define ACC CollectorBaseIndex_TestBase_LibB + 1
#endif

#undef ACC

#endif
}
