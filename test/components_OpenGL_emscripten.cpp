#include "Modules/moduleslib.h"
#ifdef BUILD_OpenGL
#    include "OpenGL/opengllib.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/clientlib.h"
#endif
#ifdef BUILD_Tools
#    include "toolslib.h"
#endif
#ifdef BUILD_Modules
#    include "Modules/moduleslib.h"
#endif
#ifdef BUILD_EmscriptenInput
#    include "emscripteninputlib.h"
#endif
#ifdef BUILD_ClientTools
#    include "Madgine_Tools/clienttoolslib.h"
#endif
#ifdef BUILD_OpenGLTools
#    include "OpenGL_Tools/opengltoolslib.h"
#endif
#ifdef BUILD_Base
#    include "Madgine/baselib.h"
#endif
#ifdef BUILD_Base
#    include "Madgine/app/globalapicollector.h"
#endif
#ifdef BUILD_Base
#    include "Madgine/scene/scenemanager.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/gui/guisystem.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/gui/widgets/toplevelwindow.h"
#endif
#ifdef BUILD_ClientTools
#    include "Madgine_Tools/imgui/clientimroot.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/input/inputcollector.h"
#endif
#ifdef BUILD_EmscriptenInput
#    include "emscripteninputhandler.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/render/renderercollector.h"
#endif
#ifdef BUILD_OpenGL
#    include "OpenGL/openglrenderer.h"
#endif
#ifdef BUILD_Modules
#    include "Modules/resources/resourceloadercollector.h"
#endif
#ifdef BUILD_Tools
#    include "inspector/layoutloader.h"
#endif
#ifdef BUILD_OpenGL
#    include "OpenGL/openglfontloader.h"
#endif
#ifdef BUILD_OpenGL
#    include "OpenGL/openglmeshloader.h"
#endif
#ifdef BUILD_OpenGL
#    include "OpenGL/openglshaderloader.h"
#endif
#ifdef BUILD_Base
#    include "Madgine/scene/scenecomponentcollector.h"
#endif
#ifdef BUILD_ClientTools
#    include "Madgine_Tools/imgui/immanager.h"
#endif
#ifdef BUILD_OpenGLTools
#    include "OpenGL_Tools/imgui/openglimmanager.h"
#endif
#ifdef BUILD_Tools
#    include "toolscollector.h"
#endif
#ifdef BUILD_Tools
#    include "inspector/functiontool.h"
#endif
#ifdef BUILD_Tools
#    include "inspector/inspector.h"
#endif
#ifdef BUILD_Tools
#    include "metrics/metrics.h"
#endif
#ifdef BUILD_Tools
#    include "profiler/profiler.h"
#endif
#ifdef BUILD_Tools
#    include "renderer/imguidemo.h"
#endif
#ifdef BUILD_Tools
#    include "testtool/testtool.h"
#endif
#ifdef BUILD_ClientTools
#    include "Madgine_Tools/guieditor/guieditor.h"
#endif
#ifdef BUILD_OpenGLTools
#    include "OpenGL_Tools/sceneeditor/sceneeditor.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/ui/gamehandler.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/ui/guihandler.h"
#endif
#ifdef BUILD_TestShared
#    include "uniquecomponent/uniquecomponentshared.h"
#endif
#ifdef BUILD_LibA
#    include "uniquecomponent/libA.h"
#endif
#ifdef BUILD_LibB
#    include "uniquecomponent/libB.h"
#endif


namespace Engine {

#ifdef BUILD_Base
template <>
const std::vector<const Engine::MetaTable *> &Engine::App::GlobalAPICollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_Base
		&table<Engine::Scene::SceneManager>(),
#    endif
#    ifdef BUILD_Client
		&table<Engine::GUI::GUISystem>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::App::GlobalAPICollector::Registry::F> Engine::App::GlobalAPICollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_Base
		createComponent<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>,
#    endif
#    ifdef BUILD_Client
		createComponent<Engine::GUI::GUISystem>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_Base
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Base = ACC;
template <>
size_t component_index<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>() { return CollectorBaseIndex_GlobalAPIBase_Base + 0; }
template <>
size_t component_index<Engine::Scene::SceneManager>() { return CollectorBaseIndex_GlobalAPIBase_Base + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_GlobalAPIBase_Base + 1
#    endif
#    ifdef BUILD_Client
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Client = ACC;
template <>
size_t component_index<Engine::GUI::GUISystem>() { return CollectorBaseIndex_GlobalAPIBase_Client + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_GlobalAPIBase_Client + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Client
template <>
const std::vector<const Engine::MetaTable *> &Engine::GUI::TopLevelWindowCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_ClientTools
		&table<Engine::Tools::ClientImRoot>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::GUI::TopLevelWindowCollector::Registry::F> Engine::GUI::TopLevelWindowCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_ClientTools
		createComponent<Engine::Tools::ClientImRoot>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_ClientTools
constexpr size_t CollectorBaseIndex_TopLevelWindowComponentBase_ClientTools = ACC;
template <>
size_t component_index<Engine::Tools::ClientImRoot>() { return CollectorBaseIndex_TopLevelWindowComponentBase_ClientTools + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_TopLevelWindowComponentBase_ClientTools + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Client
template <>
const std::vector<const Engine::MetaTable *> &Engine::Input::InputHandlerCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_EmscriptenInput
		&table<Engine::Input::EmscriptenInputHandler>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::Input::InputHandlerCollector::Registry::F> Engine::Input::InputHandlerCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_EmscriptenInput
		createComponent<Engine::Input::EmscriptenInputHandler>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_EmscriptenInput
constexpr size_t CollectorBaseIndex_InputHandler_EmscriptenInput = ACC;
template <>
size_t component_index<Engine::Input::EmscriptenInputHandler>() { return CollectorBaseIndex_InputHandler_EmscriptenInput + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_InputHandler_EmscriptenInput + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Client
template <>
const std::vector<const Engine::MetaTable *> &Engine::Render::RendererCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_OpenGL
		&table<Engine::Render::OpenGLRenderer>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::Render::RendererCollector::Registry::F> Engine::Render::RendererCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_OpenGL
		createComponent<Engine::Render::OpenGLRenderer>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_OpenGL
constexpr size_t CollectorBaseIndex_RendererBase_OpenGL = ACC;
template <>
size_t component_index<Engine::Render::OpenGLRenderer>() { return CollectorBaseIndex_RendererBase_OpenGL + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_RendererBase_OpenGL + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Modules
template <>
const std::vector<const Engine::MetaTable *> &Engine::Resources::ResourceLoaderCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_Tools
		&table<Engine::Tools::LayoutLoader>(),
#    endif
#    ifdef BUILD_OpenGL
		&table<Engine::Render::OpenGLFontLoader>(),
		&table<Engine::Render::OpenGLMeshLoader>(),
		&table<Engine::Render::OpenGLShaderLoader>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::Resources::ResourceLoaderCollector::Registry::F> Engine::Resources::ResourceLoaderCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_Tools
		createComponent<Engine::Tools::LayoutLoader>,
#    endif
#    ifdef BUILD_OpenGL
		createComponent<Engine::Render::OpenGLFontLoader>,
		createComponent<Engine::Render::OpenGLMeshLoader>,
		createComponent<Engine::Render::OpenGLShaderLoader>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_Tools
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_Tools = ACC;
template <>
size_t component_index<Engine::Tools::LayoutLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Tools + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_Tools + 1
#    endif
#    ifdef BUILD_OpenGL
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_OpenGL = ACC;
template <>
size_t component_index<Engine::Render::OpenGLFontLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 0; }
template <>
size_t component_index<Engine::Render::OpenGLMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
template <>
size_t component_index<Engine::Render::OpenGLShaderLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 2; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_OpenGL + 3
#    endif

#    undef ACC

#endif
#ifdef BUILD_Base
template <>
const std::vector<const Engine::MetaTable *> &Engine::Scene::SceneComponentCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {

	}; 
	return dummy;
}
template <>
std::vector<Engine::Scene::SceneComponentCollector::Registry::F> Engine::Scene::SceneComponentCollector::Registry::sComponents()
{
	return {

	}; 
}

#    define ACC 0


#    undef ACC

#endif
#ifdef BUILD_ClientTools
template <>
const std::vector<const Engine::MetaTable *> &Engine::Tools::ImManagerCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_OpenGLTools
		&table<Engine::Tools::OpenGLImManager>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::Tools::ImManagerCollector::Registry::F> Engine::Tools::ImManagerCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_OpenGLTools
		createComponent<Engine::Tools::OpenGLImManager>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_OpenGLTools
constexpr size_t CollectorBaseIndex_ImManager_OpenGLTools = ACC;
template <>
size_t component_index<Engine::Tools::OpenGLImManager>() { return CollectorBaseIndex_ImManager_OpenGLTools + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ImManager_OpenGLTools + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Tools
template <>
const std::vector<const Engine::MetaTable *> &Engine::Tools::ToolsCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_Tools
		&table<Engine::Tools::FunctionTool>(),
		&table<Engine::Tools::Inspector>(),
		&table<Engine::Tools::Metrics>(),
		&table<Engine::Tools::Profiler>(),
		&table<Engine::Tools::ImGuiDemo>(),
		&table<Engine::Tools::TestTool>(),
#    endif
#    ifdef BUILD_ClientTools
		&table<Engine::Tools::GuiEditor>(),
#    endif
#    ifdef BUILD_OpenGLTools
		&table<Engine::Tools::SceneEditor>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::Tools::ToolsCollector::Registry::F> Engine::Tools::ToolsCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_Tools
		createComponent<Engine::Tools::FunctionTool>,
		createComponent<Engine::Tools::Inspector>,
		createComponent<Engine::Tools::Metrics>,
		createComponent<Engine::Tools::Profiler>,
		createComponent<Engine::Tools::ImGuiDemo>,
		createComponent<Engine::Tools::TestTool>,
#    endif
#    ifdef BUILD_ClientTools
		createComponent<Engine::Tools::GuiEditor>,
#    endif
#    ifdef BUILD_OpenGLTools
		createComponent<Engine::Tools::SceneEditor>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_Tools
constexpr size_t CollectorBaseIndex_ToolBase_Tools = ACC;
template <>
size_t component_index<Engine::Tools::FunctionTool>() { return CollectorBaseIndex_ToolBase_Tools + 0; }
template <>
size_t component_index<Engine::Tools::Inspector>() { return CollectorBaseIndex_ToolBase_Tools + 1; }
template <>
size_t component_index<Engine::Tools::Metrics>() { return CollectorBaseIndex_ToolBase_Tools + 2; }
template <>
size_t component_index<Engine::Tools::Profiler>() { return CollectorBaseIndex_ToolBase_Tools + 3; }
template <>
size_t component_index<Engine::Tools::ImGuiDemo>() { return CollectorBaseIndex_ToolBase_Tools + 4; }
template <>
size_t component_index<Engine::Tools::TestTool>() { return CollectorBaseIndex_ToolBase_Tools + 5; }
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_Tools + 6
#    endif
#    ifdef BUILD_ClientTools
constexpr size_t CollectorBaseIndex_ToolBase_ClientTools = ACC;
template <>
size_t component_index<Engine::Tools::GuiEditor>() { return CollectorBaseIndex_ToolBase_ClientTools + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_ClientTools + 1
#    endif
#    ifdef BUILD_OpenGLTools
constexpr size_t CollectorBaseIndex_ToolBase_OpenGLTools = ACC;
template <>
size_t component_index<Engine::Tools::SceneEditor>() { return CollectorBaseIndex_ToolBase_OpenGLTools + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_OpenGLTools + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Client
template <>
const std::vector<const Engine::MetaTable *> &Engine::UI::GameHandlerCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {

	}; 
	return dummy;
}
template <>
std::vector<Engine::UI::GameHandlerCollector::Registry::F> Engine::UI::GameHandlerCollector::Registry::sComponents()
{
	return {

	}; 
}

#    define ACC 0


#    undef ACC

#endif
#ifdef BUILD_Client
template <>
const std::vector<const Engine::MetaTable *> &Engine::UI::GuiHandlerCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {

	}; 
	return dummy;
}
template <>
std::vector<Engine::UI::GuiHandlerCollector::Registry::F> Engine::UI::GuiHandlerCollector::Registry::sComponents()
{
	return {

	}; 
}

#    define ACC 0


#    undef ACC

#endif
#ifdef BUILD_TestShared
template <>
const std::vector<const Engine::MetaTable *> &Test::TestCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_LibA
		&table<LibAComponent>(),
#    endif
#    ifdef BUILD_LibB
		&table<LibBComponent>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Test::TestCollector::Registry::F> Test::TestCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_LibA
		createComponent<LibAComponent>,
#    endif
#    ifdef BUILD_LibB
		createComponent<LibBComponent>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_LibA
constexpr size_t CollectorBaseIndex_TestBase_LibA = ACC;
template <>
size_t component_index<LibAComponent>() { return CollectorBaseIndex_TestBase_LibA + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_TestBase_LibA + 1
#    endif
#    ifdef BUILD_LibB
constexpr size_t CollectorBaseIndex_TestBase_LibB = ACC;
template <>
size_t component_index<LibBComponent>() { return CollectorBaseIndex_TestBase_LibB + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_TestBase_LibB + 1
#    endif

#    undef ACC

#endif
}
