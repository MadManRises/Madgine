#include "Modules/moduleslib.h"
#include "Modules/uniquecomponent/uniquecomponentregistry.h"
#if defined(BUILD_MeshLoader)
#include "meshloaderlib.h"
#endif
#if defined(BUILD_Widgets)
#include "Madgine/widgetslib.h"
#endif
#if defined(BUILD_FontLoader)
#include "fontloaderlib.h"
#endif
#if defined(BUILD_UI)
#include "Madgine/uilib.h"
#endif
#if defined(BUILD_ImageLoader)
#include "imageloaderlib.h"
#endif
#if defined(BUILD_ClickBrick)
#include "clickbricklib.h"
#endif
#if defined(BUILD_Client)
#include "Madgine/clientlib.h"
#endif
#if defined(BUILD_Base)
#include "Madgine/baselib.h"
#endif
#if defined(BUILD_Tools)
#include "toolslib.h"
#endif
#if defined(BUILD_OpenGL)
#include "OpenGL/opengllib.h"
#endif
#if defined(BUILD_Scene)
#include "Madgine/scenelib.h"
#endif
#if defined(BUILD_WidgetsTools)
#include "Madgine_Tools/widgetstoolslib.h"
#endif
#if defined(BUILD_SceneRendererTools)
#include "Madgine_Tools/scenerenderertoolslib.h"
#endif
#if defined(BUILD_OpenGLTools)
#include "OpenGL_Tools/opengltoolslib.h"
#endif
#if defined(BUILD_SkeletonLoader)
#include "skeletonloaderlib.h"
#endif
#if defined(BUILD_AnimationLoader)
#include "animationloaderlib.h"
#endif

#if defined(BUILD_Resources)
#include "Madgine/resources/resourceloadercollector.h"
#endif
#if defined(BUILD_TestShared)
#include "uniquecomponent/uniquecomponentshared.h"
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibA)
#include "uniquecomponent/libA.h"
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibB)
#include "uniquecomponent/libB.h"
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
#include "meshloader.h"
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
#include "fontloader.h"
#endif
#if defined(BUILD_UI)
#include "Madgine/ui/handlercollector.h"
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
#include "imageloader.h"
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
#include "gamehandler.h"
#include "gamemanager.h"
#include "gameoverhandler.h"
#include "mainmenuhandler.h"
#endif
#if defined(BUILD_Client)
#include "Madgine/render/rendercontextcollector.h"
#include "Madgine/window/mainwindowcomponentcollector.h"
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
#include "Madgine/widgets/widgetmanager.h"
#endif
#if defined(BUILD_UI) && defined(BUILD_Client)
#include "Madgine/ui/uimanager.h"
#endif
#if defined(BUILD_Base)
#include "Madgine/app/globalapicollector.h"
#endif
#if defined(BUILD_Tools)
#include "inspector/functiontool.h"
#include "inspector/inspector.h"
#include "metrics/metrics.h"
#include "profiler/profiler.h"
#include "project/projectmanager.h"
#include "renderer/imguidemo.h"
#include "testtool/testtool.h"
#include "toolscollector.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_Resources)
#include "inspector/layoutloader.h"
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
#include "OpenGL/openglmeshloader.h"
#include "OpenGL/openglprogramloader.h"
#include "OpenGL/openglshaderloader.h"
#include "OpenGL/opengltextureloader.h"
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
#include "OpenGL/openglrendercontext.h"
#endif
#if defined(BUILD_Scene)
#include "Madgine/scene/entity/components/animation.h"
#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entitycomponentcollector.h"
#include "Madgine/scene/scenecomponentcollector.h"
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
#include "brick.h"
#endif
#if defined(BUILD_Scene) && defined(BUILD_Base)
#include "Madgine/scene/scenemanager.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_WidgetsTools)
#include "Madgine_Tools/guieditor/guieditor.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_SceneRendererTools)
#include "Madgine_Tools/sceneeditor/sceneeditor.h"
#endif
#if defined(BUILD_OpenGLTools) && defined(BUILD_Client)
#include "OpenGL_Tools/imgui/openglimroot.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_OpenGLTools)
#include "OpenGL_Tools/opengltoolconfig.h"
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
#include "skeletonloader.h"
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
#include "animationloader.h"
#endif


namespace Engine{
#if defined(BUILD_Base)
template <>
std::vector<Engine::App::GlobalAPIRegistry::F> Engine::App::GlobalAPIRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Scene) && defined(BUILD_Base)
		createComponent<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>,
#endif
#if defined(BUILD_Base)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_Scene) && defined(BUILD_Base)
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Scene = ACC;
#endif
#if defined(BUILD_Scene) && defined(BUILD_Base)
template <>
size_t component_index<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>() { return CollectorBaseIndex_GlobalAPIBase_Scene + 0; }
#endif
#if defined(BUILD_Scene) && defined(BUILD_Base)
template <>
size_t component_index<Engine::Scene::SceneManager>() { return CollectorBaseIndex_GlobalAPIBase_Scene + 0; }
#endif
#if defined(BUILD_Scene) && defined(BUILD_Base)
#        undef ACC
#        define ACC CollectorBaseIndex_GlobalAPIBase_Scene + 1
#endif
#if defined(BUILD_Base)

#    undef ACC

#endif
#if defined(BUILD_Client)
template <>
std::vector<Engine::Render::RenderContextRegistry::F> Engine::Render::RenderContextRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
		createComponent<Engine::Render::OpenGLRenderContext>,
#endif
#if defined(BUILD_Client)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
constexpr size_t CollectorBaseIndex_RenderContext_OpenGL = ACC;
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
template <>
size_t component_index<Engine::Render::OpenGLRenderContext>() { return CollectorBaseIndex_RenderContext_OpenGL + 0; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
#        undef ACC
#        define ACC CollectorBaseIndex_RenderContext_OpenGL + 1
#endif
#if defined(BUILD_Client)

#    undef ACC

#endif
#if defined(BUILD_Resources)
template <>
std::vector<Engine::Resources::ResourceLoaderRegistry::F> Engine::Resources::ResourceLoaderRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
		createComponent<Engine::Render::MeshLoader>,
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
		createComponent<Engine::Resources::ImageLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		createComponent<Engine::Render::OpenGLMeshLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		createComponent<Engine::Render::OpenGLProgramLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		createComponent<Engine::Render::OpenGLShaderLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		createComponent<Engine::Render::OpenGLTextureLoader>,
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
		createComponent<Engine::Render::FontLoader>,
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
		createComponent<Engine::Render::SkeletonLoader>,
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
		createComponent<Engine::Render::AnimationLoader>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_Resources)
		createComponent<Engine::Tools::LayoutLoader>,
#endif
#if defined(BUILD_Resources)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_MeshLoader = ACC;
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::MeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_MeshLoader + 0; }
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_MeshLoader + 1
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_ImageLoader = ACC;
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Resources::ImageLoader>() { return CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 0; }
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 1
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_OpenGL = ACC;
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::OpenGLMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 0; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::GPUMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 0; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::OpenGLProgramLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::ProgramLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::OpenGLShaderLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 2; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::OpenGLTextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 3; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::TextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 3; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_OpenGL + 4
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_FontLoader = ACC;
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::FontLoader>() { return CollectorBaseIndex_ResourceLoaderBase_FontLoader + 0; }
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_FontLoader + 1
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader = ACC;
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::SkeletonLoader>() { return CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader + 0; }
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader + 1
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_AnimationLoader = ACC;
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::AnimationLoader>() { return CollectorBaseIndex_ResourceLoaderBase_AnimationLoader + 0; }
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_AnimationLoader + 1
#endif
#if defined(BUILD_Tools) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_Tools = ACC;
#endif
#if defined(BUILD_Tools) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Tools::LayoutLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Tools + 0; }
#endif
#if defined(BUILD_Tools) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_Tools + 1
#endif
#if defined(BUILD_Resources)

#    undef ACC

#endif
#if defined(BUILD_Scene)
template <>
std::vector<Engine::Scene::Entity::EntityComponentListRegistry::F> Engine::Scene::Entity::EntityComponentListRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Scene)
		createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>>,
#endif
#if defined(BUILD_Scene)
		createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Mesh>>,
#endif
#if defined(BUILD_Scene)
		createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Skeleton>>,
#endif
#if defined(BUILD_Scene)
		createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Transform>>,
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
		createComponent<Engine::Scene::Entity::EntityComponentList<ClickBrick::Scene::Brick>>,
#endif
#if defined(BUILD_Scene)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_Scene)
constexpr size_t CollectorBaseIndex_EntityComponentListBase_Scene = ACC;
#endif
#if defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 0; }
#endif
#if defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Mesh>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 1; }
#endif
#if defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Skeleton>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 2; }
#endif
#if defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Transform>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 3; }
#endif
#if defined(BUILD_Scene)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_Scene + 4
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
constexpr size_t CollectorBaseIndex_EntityComponentListBase_ClickBrick = ACC;
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<ClickBrick::Scene::Brick>>() { return CollectorBaseIndex_EntityComponentListBase_ClickBrick + 0; }
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_ClickBrick + 1
#endif
#if defined(BUILD_Scene)

#    undef ACC

#endif
#if defined(BUILD_Scene)
template <>
std::vector<Engine::Scene::Entity::EntityComponentRegistry::F> Engine::Scene::Entity::EntityComponentRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Scene)
		createComponent<Engine::Scene::Entity::Animation>,
#endif
#if defined(BUILD_Scene)
		createComponent<Engine::Scene::Entity::Mesh>,
#endif
#if defined(BUILD_Scene)
		createComponent<Engine::Scene::Entity::Skeleton>,
#endif
#if defined(BUILD_Scene)
		createComponent<Engine::Scene::Entity::Transform>,
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
		createComponent<ClickBrick::Scene::Brick>,
#endif
#if defined(BUILD_Scene)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_Scene)
constexpr size_t CollectorBaseIndex_EntityComponentBase_Scene = ACC;
#endif
#if defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::Animation>() { return CollectorBaseIndex_EntityComponentBase_Scene + 0; }
#endif
#if defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::Mesh>() { return CollectorBaseIndex_EntityComponentBase_Scene + 1; }
#endif
#if defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::Skeleton>() { return CollectorBaseIndex_EntityComponentBase_Scene + 2; }
#endif
#if defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::Transform>() { return CollectorBaseIndex_EntityComponentBase_Scene + 3; }
#endif
#if defined(BUILD_Scene)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentBase_Scene + 4
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
constexpr size_t CollectorBaseIndex_EntityComponentBase_ClickBrick = ACC;
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
template <>
size_t component_index<ClickBrick::Scene::Brick>() { return CollectorBaseIndex_EntityComponentBase_ClickBrick + 0; }
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentBase_ClickBrick + 1
#endif
#if defined(BUILD_Scene)

#    undef ACC

#endif
#if defined(BUILD_Scene)
template <>
std::vector<Engine::Scene::SceneComponentRegistry::F> Engine::Scene::SceneComponentRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Scene)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_Scene)

#    undef ACC

#endif
#if defined(BUILD_Tools)
template <>
std::vector<Engine::Tools::ToolsRegistry::F> Engine::Tools::ToolsRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::FunctionTool>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::Inspector>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::Metrics>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::Profiler>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::ProjectManager>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::ImGuiDemo>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::TestTool>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_OpenGLTools)
		createComponent<Engine::Tools::OpenGLToolConfig>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_SceneRendererTools)
		createComponent<Engine::Tools::SceneEditor>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_WidgetsTools)
		createComponent<Engine::Tools::GuiEditor>,
#endif
#if defined(BUILD_Tools)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_Tools)
constexpr size_t CollectorBaseIndex_ToolBase_Tools = ACC;
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::FunctionTool>() { return CollectorBaseIndex_ToolBase_Tools + 0; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::Inspector>() { return CollectorBaseIndex_ToolBase_Tools + 1; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::Metrics>() { return CollectorBaseIndex_ToolBase_Tools + 2; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::Profiler>() { return CollectorBaseIndex_ToolBase_Tools + 3; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::ProjectManager>() { return CollectorBaseIndex_ToolBase_Tools + 4; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::ImGuiDemo>() { return CollectorBaseIndex_ToolBase_Tools + 5; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::TestTool>() { return CollectorBaseIndex_ToolBase_Tools + 6; }
#endif
#if defined(BUILD_Tools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_Tools + 7
#endif
#if defined(BUILD_Tools) && defined(BUILD_OpenGLTools)
constexpr size_t CollectorBaseIndex_ToolBase_OpenGLTools = ACC;
#endif
#if defined(BUILD_Tools) && defined(BUILD_OpenGLTools)
template <>
size_t component_index<Engine::Tools::OpenGLToolConfig>() { return CollectorBaseIndex_ToolBase_OpenGLTools + 0; }
#endif
#if defined(BUILD_Tools) && defined(BUILD_OpenGLTools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_OpenGLTools + 1
#endif
#if defined(BUILD_Tools) && defined(BUILD_SceneRendererTools)
constexpr size_t CollectorBaseIndex_ToolBase_SceneRendererTools = ACC;
#endif
#if defined(BUILD_Tools) && defined(BUILD_SceneRendererTools)
template <>
size_t component_index<Engine::Tools::SceneEditor>() { return CollectorBaseIndex_ToolBase_SceneRendererTools + 0; }
#endif
#if defined(BUILD_Tools) && defined(BUILD_SceneRendererTools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_SceneRendererTools + 1
#endif
#if defined(BUILD_Tools) && defined(BUILD_WidgetsTools)
constexpr size_t CollectorBaseIndex_ToolBase_WidgetsTools = ACC;
#endif
#if defined(BUILD_Tools) && defined(BUILD_WidgetsTools)
template <>
size_t component_index<Engine::Tools::GuiEditor>() { return CollectorBaseIndex_ToolBase_WidgetsTools + 0; }
#endif
#if defined(BUILD_Tools) && defined(BUILD_WidgetsTools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_WidgetsTools + 1
#endif
#if defined(BUILD_Tools)

#    undef ACC

#endif
#if defined(BUILD_UI)
template <>
std::vector<Engine::UI::GameHandlerRegistry::F> Engine::UI::GameHandlerRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
		createComponent<ClickBrick::UI::GameManager>,
#endif
#if defined(BUILD_UI)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
constexpr size_t CollectorBaseIndex_GameHandlerBase_ClickBrick = ACC;
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
template <>
size_t component_index<ClickBrick::UI::GameManager>() { return CollectorBaseIndex_GameHandlerBase_ClickBrick + 0; }
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
#        undef ACC
#        define ACC CollectorBaseIndex_GameHandlerBase_ClickBrick + 1
#endif
#if defined(BUILD_UI)

#    undef ACC

#endif
#if defined(BUILD_UI)
template <>
std::vector<Engine::UI::GuiHandlerRegistry::F> Engine::UI::GuiHandlerRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
		createComponent<ClickBrick::UI::MainMenuHandler>,
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
		createComponent<ClickBrick::UI::GameHandler>,
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
		createComponent<ClickBrick::UI::GameOverHandler>,
#endif
#if defined(BUILD_UI)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
constexpr size_t CollectorBaseIndex_GuiHandlerBase_ClickBrick = ACC;
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
template <>
size_t component_index<ClickBrick::UI::MainMenuHandler>() { return CollectorBaseIndex_GuiHandlerBase_ClickBrick + 0; }
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
template <>
size_t component_index<ClickBrick::UI::GameHandler>() { return CollectorBaseIndex_GuiHandlerBase_ClickBrick + 1; }
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
template <>
size_t component_index<ClickBrick::UI::GameOverHandler>() { return CollectorBaseIndex_GuiHandlerBase_ClickBrick + 2; }
#endif
#if defined(BUILD_UI) && defined(BUILD_ClickBrick)
#        undef ACC
#        define ACC CollectorBaseIndex_GuiHandlerBase_ClickBrick + 3
#endif
#if defined(BUILD_UI)

#    undef ACC

#endif
#if defined(BUILD_Client)
template <>
std::vector<Engine::Window::MainWindowComponentRegistry::F> Engine::Window::MainWindowComponentRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_OpenGLTools) && defined(BUILD_Client)
		createComponent<Engine::Tools::OpenGLImRoot>,
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
		createComponent<Engine::Widgets::WidgetManager>,
#endif
#if defined(BUILD_UI) && defined(BUILD_Client)
		createComponent<Engine::UI::UIManager>,
#endif
#if defined(BUILD_Client)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_OpenGLTools) && defined(BUILD_Client)
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_OpenGLTools = ACC;
#endif
#if defined(BUILD_OpenGLTools) && defined(BUILD_Client)
template <>
size_t component_index<Engine::Tools::OpenGLImRoot>() { return CollectorBaseIndex_MainWindowComponentBase_OpenGLTools + 0; }
#endif
#if defined(BUILD_OpenGLTools) && defined(BUILD_Client)
template <>
size_t component_index<Engine::Tools::ClientImRoot>() { return CollectorBaseIndex_MainWindowComponentBase_OpenGLTools + 0; }
#endif
#if defined(BUILD_OpenGLTools) && defined(BUILD_Client)
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_OpenGLTools + 1
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_Widgets = ACC;
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
template <>
size_t component_index<Engine::Widgets::WidgetManager>() { return CollectorBaseIndex_MainWindowComponentBase_Widgets + 0; }
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_Widgets + 1
#endif
#if defined(BUILD_UI) && defined(BUILD_Client)
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_UI = ACC;
#endif
#if defined(BUILD_UI) && defined(BUILD_Client)
template <>
size_t component_index<Engine::UI::UIManager>() { return CollectorBaseIndex_MainWindowComponentBase_UI + 0; }
#endif
#if defined(BUILD_UI) && defined(BUILD_Client)
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_UI + 1
#endif
#if defined(BUILD_Client)

#    undef ACC

#endif
#if defined(BUILD_TestShared)
template <>
std::vector<Test::TestRegistry::F> Test::TestRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibA)
		createComponent<LibAComponent>,
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibB)
		createComponent<LibBComponent>,
#endif
#if defined(BUILD_TestShared)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibA)
constexpr size_t CollectorBaseIndex_TestBase_LibA = ACC;
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibA)
template <>
size_t component_index<LibAComponent>() { return CollectorBaseIndex_TestBase_LibA + 0; }
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibA)
#        undef ACC
#        define ACC CollectorBaseIndex_TestBase_LibA + 1
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibB)
constexpr size_t CollectorBaseIndex_TestBase_LibB = ACC;
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibB)
template <>
size_t component_index<LibBComponent>() { return CollectorBaseIndex_TestBase_LibB + 0; }
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibB)
#        undef ACC
#        define ACC CollectorBaseIndex_TestBase_LibB + 1
#endif
#if defined(BUILD_TestShared)

#    undef ACC

#endif
}
