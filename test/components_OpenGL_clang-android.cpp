#include "Modules/moduleslib.h"
#ifdef BUILD_Modules
#    include "Modules/moduleslib.h"
#endif
#ifdef BUILD_Tools
#    include "toolslib.h"
#endif
#ifdef BUILD_OpenGL
#    include "OpenGL/opengllib.h"
#endif
#ifdef BUILD_SceneRendererTools
#    include "Madgine_Tools/scenerenderertoolslib.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scenelib.h"
#endif
#ifdef BUILD_WidgetsTools
#    include "Madgine_Tools/widgetstoolslib.h"
#endif
#ifdef BUILD_UI
#    include "Madgine/uilib.h"
#endif
#ifdef BUILD_ClickBrick
#    include "clickbricklib.h"
#endif
#ifdef BUILD_Widgets
#    include "Madgine/widgetslib.h"
#endif
#ifdef BUILD_AndroidInput
#    include "androidinputlib.h"
#endif
#ifdef BUILD_OpenGLTools
#    include "OpenGL_Tools/opengltoolslib.h"
#endif
#ifdef BUILD_ImageLoader
#    include "imageloaderlib.h"
#endif
#ifdef BUILD_Base
#    include "Madgine/baselib.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/clientlib.h"
#endif
#ifdef BUILD_Base
#    include "Madgine/app/globalapicollector.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/scenemanager.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/gui/toplevelwindowcomponentcollector.h"
#endif
#ifdef BUILD_OpenGLTools
#    include "OpenGL_Tools/imgui/openglimroot.h"
#endif
#ifdef BUILD_Widgets
#    include "Madgine/widgets/widgetmanager.h"
#endif
#ifdef BUILD_UI
#    include "Madgine/ui/uimanager.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/input/inputcollector.h"
#endif
#ifdef BUILD_AndroidInput
#    include "androidinputhandler.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/render/rendercontextcollector.h"
#endif
#ifdef BUILD_OpenGL
#    include "OpenGL/openglrendercontext.h"
#endif
#ifdef BUILD_Modules
#    include "Modules/resources/resourceloadercollector.h"
#endif
#ifdef BUILD_ImageLoader
#    include "imageloader.h"
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
#    include "OpenGL/openglprogramloader.h"
#endif
#ifdef BUILD_OpenGL
#    include "OpenGL/openglshaderloader.h"
#endif
#ifdef BUILD_OpenGL
#    include "OpenGL/opengltextureloader.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/scenecomponentcollector.h"
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
#    include "project/projectmanager.h"
#endif
#ifdef BUILD_Tools
#    include "renderer/imguidemo.h"
#endif
#ifdef BUILD_Tools
#    include "testtool/testtool.h"
#endif
#ifdef BUILD_OpenGLTools
#    include "OpenGL_Tools/opengltoolconfig.h"
#endif
#ifdef BUILD_WidgetsTools
#    include "Madgine_Tools/guieditor/guieditor.h"
#endif
#ifdef BUILD_SceneRendererTools
#    include "Madgine_Tools/sceneeditor/sceneeditor.h"
#endif
#ifdef BUILD_UI
#    include "Madgine/ui/gamehandler.h"
#endif
#ifdef BUILD_ClickBrick
#    include "gamemanager.h"
#endif
#ifdef BUILD_UI
#    include "Madgine/ui/guihandler.h"
#endif
#ifdef BUILD_ClickBrick
#    include "mainmenuhandler.h"
#endif
#ifdef BUILD_ClickBrick
#    include "gamehandler.h"
#endif
#ifdef BUILD_ClickBrick
#    include "gameoverhandler.h"
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
#    ifdef BUILD_Scene
		&table<Engine::Scene::SceneManager>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::App::GlobalAPICollector::Registry::F> Engine::App::GlobalAPICollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_Scene
		createComponent<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_Scene
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Scene = ACC;
template <>
size_t component_index<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>() { return CollectorBaseIndex_GlobalAPIBase_Scene + 0; }
template <>
size_t component_index<Engine::Scene::SceneManager>() { return CollectorBaseIndex_GlobalAPIBase_Scene + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_GlobalAPIBase_Scene + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Client
template <>
const std::vector<const Engine::MetaTable *> &Engine::GUI::TopLevelWindowComponentCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_OpenGLTools
		&table<Engine::Tools::OpenGLImRoot>(),
#    endif
#    ifdef BUILD_Widgets
		&table<Engine::Widgets::WidgetManager>(),
#    endif
#    ifdef BUILD_UI
		&table<Engine::UI::UIManager>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::GUI::TopLevelWindowComponentCollector::Registry::F> Engine::GUI::TopLevelWindowComponentCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_OpenGLTools
		createComponent<Engine::Tools::OpenGLImRoot>,
#    endif
#    ifdef BUILD_Widgets
		createComponent<Engine::Widgets::WidgetManager>,
#    endif
#    ifdef BUILD_UI
		createComponent<Engine::UI::UIManager>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_OpenGLTools
constexpr size_t CollectorBaseIndex_TopLevelWindowComponentBase_OpenGLTools = ACC;
template <>
size_t component_index<Engine::Tools::OpenGLImRoot>() { return CollectorBaseIndex_TopLevelWindowComponentBase_OpenGLTools + 0; }
template <>
size_t component_index<Engine::Tools::ClientImRoot>() { return CollectorBaseIndex_TopLevelWindowComponentBase_OpenGLTools + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_TopLevelWindowComponentBase_OpenGLTools + 1
#    endif
#    ifdef BUILD_Widgets
constexpr size_t CollectorBaseIndex_TopLevelWindowComponentBase_Widgets = ACC;
template <>
size_t component_index<Engine::Widgets::WidgetManager>() { return CollectorBaseIndex_TopLevelWindowComponentBase_Widgets + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_TopLevelWindowComponentBase_Widgets + 1
#    endif
#    ifdef BUILD_UI
constexpr size_t CollectorBaseIndex_TopLevelWindowComponentBase_UI = ACC;
template <>
size_t component_index<Engine::UI::UIManager>() { return CollectorBaseIndex_TopLevelWindowComponentBase_UI + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_TopLevelWindowComponentBase_UI + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Client
template <>
const std::vector<const Engine::MetaTable *> &Engine::Input::InputHandlerCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_AndroidInput
		&table<Engine::Input::AndroidInputHandler>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::Input::InputHandlerCollector::Registry::F> Engine::Input::InputHandlerCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_AndroidInput
		createComponent<Engine::Input::AndroidInputHandler>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_AndroidInput
constexpr size_t CollectorBaseIndex_InputHandler_AndroidInput = ACC;
template <>
size_t component_index<Engine::Input::AndroidInputHandler>() { return CollectorBaseIndex_InputHandler_AndroidInput + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_InputHandler_AndroidInput + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Client
template <>
const std::vector<const Engine::MetaTable *> &Engine::Render::RenderContextCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_OpenGL
		&table<Engine::Render::OpenGLRenderContext>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::Render::RenderContextCollector::Registry::F> Engine::Render::RenderContextCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_OpenGL
		createComponent<Engine::Render::OpenGLRenderContext>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_OpenGL
constexpr size_t CollectorBaseIndex_RenderContext_OpenGL = ACC;
template <>
size_t component_index<Engine::Render::OpenGLRenderContext>() { return CollectorBaseIndex_RenderContext_OpenGL + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_RenderContext_OpenGL + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Modules
template <>
const std::vector<const Engine::MetaTable *> &Engine::Resources::ResourceLoaderCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_ImageLoader
		&table<Engine::Resources::ImageLoader>(),
#    endif
#    ifdef BUILD_Tools
		&table<Engine::Tools::LayoutLoader>(),
#    endif
#    ifdef BUILD_OpenGL
		&table<Engine::Render::OpenGLFontLoader>(),
		&table<Engine::Render::OpenGLMeshLoader>(),
		&table<Engine::Render::OpenGLProgramLoader>(),
		&table<Engine::Render::OpenGLShaderLoader>(),
		&table<Engine::Render::OpenGLTextureLoader>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::Resources::ResourceLoaderCollector::Registry::F> Engine::Resources::ResourceLoaderCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_ImageLoader
		createComponent<Engine::Resources::ImageLoader>,
#    endif
#    ifdef BUILD_Tools
		createComponent<Engine::Tools::LayoutLoader>,
#    endif
#    ifdef BUILD_OpenGL
		createComponent<Engine::Render::OpenGLFontLoader>,
		createComponent<Engine::Render::OpenGLMeshLoader>,
		createComponent<Engine::Render::OpenGLProgramLoader>,
		createComponent<Engine::Render::OpenGLShaderLoader>,
		createComponent<Engine::Render::OpenGLTextureLoader>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_ImageLoader
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_ImageLoader = ACC;
template <>
size_t component_index<Engine::Resources::ImageLoader>() { return CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 1
#    endif
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
size_t component_index<Engine::Render::FontLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 0; }
template <>
size_t component_index<Engine::Render::OpenGLMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
template <>
size_t component_index<Engine::Render::MeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
template <>
size_t component_index<Engine::Render::OpenGLProgramLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 2; }
template <>
size_t component_index<Engine::Render::ProgramLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 2; }
template <>
size_t component_index<Engine::Render::OpenGLShaderLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 3; }
template <>
size_t component_index<Engine::Render::OpenGLTextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 4; }
template <>
size_t component_index<Engine::Render::TextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 4; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_OpenGL + 5
#    endif

#    undef ACC

#endif
#ifdef BUILD_Scene
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
		&table<Engine::Tools::ProjectManager>(),
		&table<Engine::Tools::ImGuiDemo>(),
		&table<Engine::Tools::TestTool>(),
#    endif
#    ifdef BUILD_OpenGLTools
		&table<Engine::Tools::OpenGLToolConfig>(),
#    endif
#    ifdef BUILD_WidgetsTools
		&table<Engine::Tools::GuiEditor>(),
#    endif
#    ifdef BUILD_SceneRendererTools
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
		createComponent<Engine::Tools::ProjectManager>,
		createComponent<Engine::Tools::ImGuiDemo>,
		createComponent<Engine::Tools::TestTool>,
#    endif
#    ifdef BUILD_OpenGLTools
		createComponent<Engine::Tools::OpenGLToolConfig>,
#    endif
#    ifdef BUILD_WidgetsTools
		createComponent<Engine::Tools::GuiEditor>,
#    endif
#    ifdef BUILD_SceneRendererTools
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
size_t component_index<Engine::Tools::ProjectManager>() { return CollectorBaseIndex_ToolBase_Tools + 4; }
template <>
size_t component_index<Engine::Tools::ImGuiDemo>() { return CollectorBaseIndex_ToolBase_Tools + 5; }
template <>
size_t component_index<Engine::Tools::TestTool>() { return CollectorBaseIndex_ToolBase_Tools + 6; }
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_Tools + 7
#    endif
#    ifdef BUILD_OpenGLTools
constexpr size_t CollectorBaseIndex_ToolBase_OpenGLTools = ACC;
template <>
size_t component_index<Engine::Tools::OpenGLToolConfig>() { return CollectorBaseIndex_ToolBase_OpenGLTools + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_OpenGLTools + 1
#    endif
#    ifdef BUILD_WidgetsTools
constexpr size_t CollectorBaseIndex_ToolBase_WidgetsTools = ACC;
template <>
size_t component_index<Engine::Tools::GuiEditor>() { return CollectorBaseIndex_ToolBase_WidgetsTools + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_WidgetsTools + 1
#    endif
#    ifdef BUILD_SceneRendererTools
constexpr size_t CollectorBaseIndex_ToolBase_SceneRendererTools = ACC;
template <>
size_t component_index<Engine::Tools::SceneEditor>() { return CollectorBaseIndex_ToolBase_SceneRendererTools + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_SceneRendererTools + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_UI
template <>
const std::vector<const Engine::MetaTable *> &Engine::UI::GameHandlerCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_ClickBrick
		&table<ClickBrick::UI::GameManager>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::UI::GameHandlerCollector::Registry::F> Engine::UI::GameHandlerCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_ClickBrick
		createComponent<ClickBrick::UI::GameManager>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_ClickBrick
constexpr size_t CollectorBaseIndex_GameHandlerBase_ClickBrick = ACC;
template <>
size_t component_index<ClickBrick::UI::GameManager>() { return CollectorBaseIndex_GameHandlerBase_ClickBrick + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_GameHandlerBase_ClickBrick + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_UI
template <>
const std::vector<const Engine::MetaTable *> &Engine::UI::GuiHandlerCollector::Registry::sTables() 
{
	static std::vector<const Engine::MetaTable *> dummy = {
#    ifdef BUILD_ClickBrick
		&table<ClickBrick::UI::MainMenuHandler>(),
		&table<ClickBrick::UI::GameHandler>(),
		&table<ClickBrick::UI::GameOverHandler>(),
#    endif

	}; 
	return dummy;
}
template <>
std::vector<Engine::UI::GuiHandlerCollector::Registry::F> Engine::UI::GuiHandlerCollector::Registry::sComponents()
{
	return {
#    ifdef BUILD_ClickBrick
		createComponent<ClickBrick::UI::MainMenuHandler>,
		createComponent<ClickBrick::UI::GameHandler>,
		createComponent<ClickBrick::UI::GameOverHandler>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_ClickBrick
constexpr size_t CollectorBaseIndex_GuiHandlerBase_ClickBrick = ACC;
template <>
size_t component_index<ClickBrick::UI::MainMenuHandler>() { return CollectorBaseIndex_GuiHandlerBase_ClickBrick + 0; }
template <>
size_t component_index<ClickBrick::UI::GameHandler>() { return CollectorBaseIndex_GuiHandlerBase_ClickBrick + 1; }
template <>
size_t component_index<ClickBrick::UI::GameOverHandler>() { return CollectorBaseIndex_GuiHandlerBase_ClickBrick + 2; }
#        undef ACC
#        define ACC CollectorBaseIndex_GuiHandlerBase_ClickBrick + 3
#    endif

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
