#include "Modules/moduleslib.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentregistry.h"
#if (defined(BUILD_FontLoader))
#include "Madgine/fontloaderlib.h"
#endif
#if (defined(BUILD_Widgets))
#include "Madgine/widgetslib.h"
#endif
#if (defined(BUILD_Client))
#include "Madgine/clientlib.h"
#endif
#if (defined(BUILD_Debugger))
#include "Madgine/debuglib.h"
#endif
#if (defined(BUILD_UI))
#include "Madgine/uilib.h"
#endif
#if (defined(BUILD_Root))
#include "Madgine/rootlib.h"
#endif
#if (defined(BUILD_App))
#include "Madgine/applib.h"
#endif
#if (defined(BUILD_NodeGraphTools))
#include "Madgine_Tools/nodegraphtoolslib.h"
#endif
#if (defined(BUILD_DebuggerTools))
#include "Madgine_Tools/debugtoolslib.h"
#endif
#if (defined(BUILD_NodeGraph))
#include "Madgine/nodegraphlib.h"
#endif
#if (defined(BUILD_ClientNodesTools))
#include "Madgine/clientnodestoolslib.h"
#endif
#if (defined(BUILD_Tools))
#include "Madgine_Tools/toolslib.h"
#endif
#if (defined(BUILD_ClientTools))
#include "Madgine_Tools/clienttoolslib.h"
#endif
#if (defined(BUILD_ImageLoader))
#include "Madgine/imageloaderlib.h"
#endif
#if (defined(BUILD_MeshLoader))
#include "Madgine/meshloaderlib.h"
#endif
#if (defined(BUILD_ClickBrick))
#include "clickbricklib.h"
#endif
#if (defined(BUILD_WidgetsTools))
#include "Madgine_Tools/widgetstoolslib.h"
#endif
#if (defined(BUILD_SceneRenderer))
#include "Madgine/scenerendererlib.h"
#endif
#if (defined(BUILD_SceneRendererTools))
#include "Madgine_Tools/scenerenderertoolslib.h"
#endif
#if (defined(BUILD_SkeletonLoader))
#include "Madgine/skeletonloaderlib.h"
#endif
#if (defined(BUILD_AnimationLoader))
#include "Madgine/animationloaderlib.h"
#endif
#if (defined(BUILD_Scene))
#include "Madgine/scenelib.h"
#endif
#if (defined(BUILD_ResourcesTools))
#include "Madgine_Tools/resourcestoolslib.h"
#endif
#if (defined(BUILD_MadgineLauncherTools))
#include "Madgine_Tools/launchertoolslib.h"
#endif
#if (defined(BUILD_ClientNodes))
#include "Madgine/clientnodeslib.h"
#endif
#if (defined(BUILD_Python3))
#include "Python3/python3lib.h"
#endif
#if (defined(BUILD_Python3Tools))
#include "Madgine_Tools/Python3/python3toolslib.h"
#endif
#if (defined(BUILD_Vulkan))
#include "Vulkan/vulkanlib.h"
#endif
#if (defined(BUILD_VulkanTools))
#include "Vulkan_Tools/vulkantoolslib.h"
#endif

#if (defined(BUILD_TestShared))
#include "uniquecomponent/uniquecomponentshared.h"
#endif
#if (defined(BUILD_Client))
#include "Madgine/render/rendercontextcollector.h"
#include "Madgine/window/mainwindowcomponentcollector.h"
#endif
#if (defined(BUILD_Widgets) && defined(BUILD_Client))
#include "Madgine/widgets/widgetmanager.h"
#endif
#if (defined(BUILD_UI))
#include "Madgine/input/handlercollector.h"
#endif
#if (defined(BUILD_Root))
#include "Madgine/root/rootcomponentcollector.h"
#endif
#if (defined(BUILD_Debugger) && defined(BUILD_Root))
#include "Madgine/debug/debugger.h"
#endif
#if (defined(BUILD_App))
#include "Madgine/app/globalapicollector.h"
#endif
#if (defined(BUILD_Resources))
#include "Madgine/resources/resourceloadercollector.h"
#endif
#if (defined(BUILD_FontLoader) && defined(BUILD_Resources))
#include "Madgine/fontloader/fontloader.h"
#endif
#if (defined(BUILD_Root) && defined(BUILD_Resources))
#include "Madgine/resources/resourcemanager.h"
#endif
#if (defined(BUILD_NodeGraph))
#include "Madgine/nodegraph/nodecollector.h"
#include "Madgine/nodegraph/nodes/functionnode.h"
#include "Madgine/nodegraph/nodes/pumpnode.h"
#include "Madgine/nodegraph/nodes/testnode.h"
#include "Madgine/nodegraph/nodes/util/sendernode.cpp"
#endif
#if (defined(BUILD_NodeGraph) && defined(BUILD_Resources))
#include "Madgine/nodegraph/nodegraphloader.h"
#endif
#if (defined(BUILD_Tools))
#include "Madgine_Tools/filesystem/filebrowser.h"
#include "Madgine_Tools/inspector/functiontool.h"
#include "Madgine_Tools/inspector/inspector.h"
#include "Madgine_Tools/logviewer/logviewer.h"
#include "Madgine_Tools/metrics/metrics.h"
#include "Madgine_Tools/profiler/profiler.h"
#include "Madgine_Tools/renderer/imguidemo.h"
#include "Madgine_Tools/testtool/testtool.h"
#include "Madgine_Tools/toolscollector.h"
#endif
#if (defined(BUILD_Tools) && defined(BUILD_Root))
#include "Madgine_Tools/pluginmanager/pluginexporter.h"
#endif
#if (defined(BUILD_NodeGraphTools) && defined(BUILD_Tools))
#include "Madgine_Tools/nodegraph/nodegrapheditor.h"
#endif
#if (defined(BUILD_DebuggerTools) && defined(BUILD_Tools))
#include "Madgine_Tools/debugger/debuggerview.h"
#endif
#if (defined(BUILD_ClientNodesTools) && defined(BUILD_Tools))
#include "Madgine/client/nodes/noderenderertester.h"
#endif
#if (defined(BUILD_Tools) && defined(BUILD_ClientTools))
#include "Madgine_Tools/gamepad/gamepadviewer.h"
#include "Madgine_Tools/imgui/projectmanager.h"
#endif
#if (defined(BUILD_ImageLoader) && defined(BUILD_Resources))
#include "Madgine/imageloader/imageloader.h"
#endif
#if (defined(BUILD_MeshLoader) && defined(BUILD_Resources))
#include "Madgine/meshloader/meshloader.h"
#endif
#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
#include "gamehandler.h"
#include "gamemanager.h"
#include "gameoverhandler.h"
#include "mainmenuhandler.h"
#endif
#if (defined(BUILD_WidgetsTools) && defined(BUILD_Tools))
#include "Madgine_Tools/guieditor/guieditor.h"
#endif
#if (defined(BUILD_SceneRenderer) && defined(BUILD_Client))
#include "Madgine/render/scenemainwindowcomponent.h"
#endif
#if (defined(BUILD_SceneRendererTools) && defined(BUILD_Tools))
#include "Madgine_Tools/sceneeditor/sceneeditor.h"
#endif
#if (defined(BUILD_SkeletonLoader) && defined(BUILD_Resources))
#include "Madgine/skeletonloader/skeletonloader.h"
#endif
#if (defined(BUILD_AnimationLoader) && defined(BUILD_Resources))
#include "Madgine/animationloader/animationloader.h"
#endif
#if (defined(BUILD_Scene))
#include "Madgine/scene/entity/components/animation.h"
#include "Madgine/scene/entity/components/material.h"
#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/pointlight.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entitycomponentcollector.h"
#include "Madgine/scene/scenecomponentcollector.h"
#endif
#if (defined(BUILD_Scene) && defined(BUILD_App))
#include "Madgine/scene/scenemanager.h"
#endif
#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
#include "brick.h"
#endif
#if (defined(BUILD_ResourcesTools) && defined(BUILD_Tools))
#include "Madgine_Tools/resourcestoolconfig.h"
#endif
#if (defined(BUILD_MadgineLauncherTools) && defined(BUILD_Tools))
#include "Madgine_Tools/launcher/launchertool.h"
#endif
#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
#include "Madgine/client/nodes/framebuffernode.h"
#include "Madgine/client/nodes/gpubuffernode.h"
#include "Madgine/client/nodes/meshrenderernode.h"
#include "Madgine/client/nodes/rasterizernode.h"
#endif
#if (defined(BUILD_Python3) && defined(BUILD_Root))
#include "Python3/python3env.h"
#endif
#if (defined(BUILD_Python3) && defined(BUILD_Resources))
#include "Python3/python3fileloader.h"
#endif
#if (defined(BUILD_Python3Tools) && defined(BUILD_Tools))
#include "Madgine_Tools/Python3/python3immediatewindow.h"
#endif
#if (defined(BUILD_Vulkan) && defined(BUILD_Client))
#include "Vulkan/vulkanrendercontext.h"
#endif
#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
#include "Vulkan/vulkanmeshloader.h"
#include "Vulkan/vulkanpipelineloader.h"
#include "Vulkan/vulkanshaderloader.h"
#include "Vulkan/vulkantextureloader.h"
#endif
#if (defined(BUILD_VulkanTools) && defined(BUILD_Client))
#include "Vulkan_Tools/imgui/vulkanimroot.h"
#endif
#if (defined(BUILD_VulkanTools) && defined(BUILD_Tools))
#include "Vulkan_Tools/vulkantoolconfig.h"
#endif


namespace Engine{
#if (defined(BUILD_Root))
template <>
std::vector<Engine::Root::RootComponentRegistry::F> Engine::Root::RootComponentRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_Debugger) && defined(BUILD_Root))
		UniqueComponent::createComponent<Engine::Debug::Debugger>,
#endif

#if (defined(BUILD_Root) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Resources::ResourceManager>,
#endif

#if (defined(BUILD_Python3) && defined(BUILD_Root))
		UniqueComponent::createComponent<Engine::Scripting::Python3::Python3Environment>,
#endif

#if (defined(BUILD_Tools) && defined(BUILD_Root))
		UniqueComponent::createComponent<Engine::Tools::PluginExporter>,
#endif

#if (defined(BUILD_Root))

	}; 
}
#endif

#if (defined(BUILD_Root))

#    define ACC 0

#endif

#if (defined(BUILD_Debugger) && defined(BUILD_Root))
constexpr size_t CollectorBaseIndex_RootComponentBase_Debugger = ACC;
#endif

#if (defined(BUILD_Debugger) && defined(BUILD_Root))
template <>
size_t UniqueComponent::component_index<Engine::Debug::Debugger>() { return CollectorBaseIndex_RootComponentBase_Debugger + 0; }
#endif

#if (defined(BUILD_Debugger) && defined(BUILD_Root))
#        undef ACC
#        define ACC CollectorBaseIndex_RootComponentBase_Debugger + 1
#endif

#if (defined(BUILD_Root) && defined(BUILD_Resources))
constexpr size_t CollectorBaseIndex_RootComponentBase_Resources = ACC;
#endif

#if (defined(BUILD_Root) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Resources::ResourceManager>() { return CollectorBaseIndex_RootComponentBase_Resources + 0; }
#endif

#if (defined(BUILD_Root) && defined(BUILD_Resources))
#        undef ACC
#        define ACC CollectorBaseIndex_RootComponentBase_Resources + 1
#endif

#if (defined(BUILD_Python3) && defined(BUILD_Root))
constexpr size_t CollectorBaseIndex_RootComponentBase_Python3 = ACC;
#endif

#if (defined(BUILD_Python3) && defined(BUILD_Root))
template <>
size_t UniqueComponent::component_index<Engine::Scripting::Python3::Python3Environment>() { return CollectorBaseIndex_RootComponentBase_Python3 + 0; }
#endif

#if (defined(BUILD_Python3) && defined(BUILD_Root))
#        undef ACC
#        define ACC CollectorBaseIndex_RootComponentBase_Python3 + 1
#endif

#if (defined(BUILD_Tools) && defined(BUILD_Root))
constexpr size_t CollectorBaseIndex_RootComponentBase_Tools = ACC;
#endif

#if (defined(BUILD_Tools) && defined(BUILD_Root))
template <>
size_t UniqueComponent::component_index<Engine::Tools::PluginExporter>() { return CollectorBaseIndex_RootComponentBase_Tools + 0; }
#endif

#if (defined(BUILD_Tools) && defined(BUILD_Root))
#        undef ACC
#        define ACC CollectorBaseIndex_RootComponentBase_Tools + 1
#endif

#if (defined(BUILD_Root))

#    undef ACC

#endif

#if (defined(BUILD_Resources))
template <>
std::vector<Engine::Resources::ResourceLoaderRegistry::F> Engine::Resources::ResourceLoaderRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_NodeGraph) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::NodeGraph::NodeGraphLoader>,
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Render::VulkanMeshLoader>,
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Render::VulkanPipelineLoader>,
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Render::VulkanTextureLoader>,
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Render::VulkanShaderLoader>,
#endif

#if (defined(BUILD_Python3) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Scripting::Python3::Python3FileLoader>,
#endif

#if (defined(BUILD_AnimationLoader) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Render::AnimationLoader>,
#endif

#if (defined(BUILD_FontLoader) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Render::FontLoader>,
#endif

#if (defined(BUILD_ImageLoader) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Resources::ImageLoader>,
#endif

#if (defined(BUILD_MeshLoader) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Render::MeshLoader>,
#endif

#if (defined(BUILD_SkeletonLoader) && defined(BUILD_Resources))
		UniqueComponent::createComponent<Engine::Render::SkeletonLoader>,
#endif

#if (defined(BUILD_Resources))

	}; 
}
#endif

#if (defined(BUILD_Resources))

#    define ACC 0

#endif

#if (defined(BUILD_NodeGraph) && defined(BUILD_Resources))
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_NodeGraph = ACC;
#endif

#if (defined(BUILD_NodeGraph) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::NodeGraphLoader>() { return CollectorBaseIndex_ResourceLoaderBase_NodeGraph + 0; }
#endif

#if (defined(BUILD_NodeGraph) && defined(BUILD_Resources))
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_NodeGraph + 1
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_Vulkan = ACC;
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::VulkanMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Vulkan + 0; }
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::GPUMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Vulkan + 0; }
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::VulkanPipelineLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Vulkan + 1; }
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::PipelineLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Vulkan + 1; }
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::VulkanTextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Vulkan + 2; }
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::TextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Vulkan + 2; }
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::VulkanShaderLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Vulkan + 3; }
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Resources))
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_Vulkan + 4
#endif

#if (defined(BUILD_Python3) && defined(BUILD_Resources))
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_Python3 = ACC;
#endif

#if (defined(BUILD_Python3) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Scripting::Python3::Python3FileLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Python3 + 0; }
#endif

#if (defined(BUILD_Python3) && defined(BUILD_Resources))
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_Python3 + 1
#endif

#if (defined(BUILD_AnimationLoader) && defined(BUILD_Resources))
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_AnimationLoader = ACC;
#endif

#if (defined(BUILD_AnimationLoader) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::AnimationLoader>() { return CollectorBaseIndex_ResourceLoaderBase_AnimationLoader + 0; }
#endif

#if (defined(BUILD_AnimationLoader) && defined(BUILD_Resources))
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_AnimationLoader + 1
#endif

#if (defined(BUILD_FontLoader) && defined(BUILD_Resources))
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_FontLoader = ACC;
#endif

#if (defined(BUILD_FontLoader) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::FontLoader>() { return CollectorBaseIndex_ResourceLoaderBase_FontLoader + 0; }
#endif

#if (defined(BUILD_FontLoader) && defined(BUILD_Resources))
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_FontLoader + 1
#endif

#if (defined(BUILD_ImageLoader) && defined(BUILD_Resources))
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_ImageLoader = ACC;
#endif

#if (defined(BUILD_ImageLoader) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Resources::ImageLoader>() { return CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 0; }
#endif

#if (defined(BUILD_ImageLoader) && defined(BUILD_Resources))
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 1
#endif

#if (defined(BUILD_MeshLoader) && defined(BUILD_Resources))
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_MeshLoader = ACC;
#endif

#if (defined(BUILD_MeshLoader) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::MeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_MeshLoader + 0; }
#endif

#if (defined(BUILD_MeshLoader) && defined(BUILD_Resources))
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_MeshLoader + 1
#endif

#if (defined(BUILD_SkeletonLoader) && defined(BUILD_Resources))
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader = ACC;
#endif

#if (defined(BUILD_SkeletonLoader) && defined(BUILD_Resources))
template <>
size_t UniqueComponent::component_index<Engine::Render::SkeletonLoader>() { return CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader + 0; }
#endif

#if (defined(BUILD_SkeletonLoader) && defined(BUILD_Resources))
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader + 1
#endif

#if (defined(BUILD_Resources))

#    undef ACC

#endif

#if (defined(BUILD_Client))
template <>
std::vector<Engine::Render::RenderContextRegistry::F> Engine::Render::RenderContextRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Client))
		UniqueComponent::createComponent<Engine::Render::VulkanRenderContext>,
#endif

#if (defined(BUILD_Client))

	}; 
}
#endif

#if (defined(BUILD_Client))

#    define ACC 0

#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Client))
constexpr size_t CollectorBaseIndex_RenderContext_Vulkan = ACC;
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Client))
template <>
size_t UniqueComponent::component_index<Engine::Render::VulkanRenderContext>() { return CollectorBaseIndex_RenderContext_Vulkan + 0; }
#endif

#if (defined(BUILD_Vulkan) && defined(BUILD_Client))
#        undef ACC
#        define ACC CollectorBaseIndex_RenderContext_Vulkan + 1
#endif

#if (defined(BUILD_Client))

#    undef ACC

#endif

#if (defined(BUILD_Client))
template <>
std::vector<Engine::Window::MainWindowComponentRegistry::F> Engine::Window::MainWindowComponentRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_SceneRenderer) && defined(BUILD_Client))
		UniqueComponent::createComponent<Engine::Render::SceneMainWindowComponent>,
#endif

#if (defined(BUILD_Widgets) && defined(BUILD_Client))
		UniqueComponent::createComponent<Engine::Widgets::WidgetManager>,
#endif

#if (defined(BUILD_VulkanTools) && defined(BUILD_Client))
		UniqueComponent::createComponent<Engine::Tools::VulkanImRoot>,
#endif

#if (defined(BUILD_Client))

	}; 
}
#endif

#if (defined(BUILD_Client))

#    define ACC 0

#endif

#if (defined(BUILD_SceneRenderer) && defined(BUILD_Client))
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_SceneRenderer = ACC;
#endif

#if (defined(BUILD_SceneRenderer) && defined(BUILD_Client))
template <>
size_t UniqueComponent::component_index<Engine::Render::SceneMainWindowComponent>() { return CollectorBaseIndex_MainWindowComponentBase_SceneRenderer + 0; }
#endif

#if (defined(BUILD_SceneRenderer) && defined(BUILD_Client))
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_SceneRenderer + 1
#endif

#if (defined(BUILD_Widgets) && defined(BUILD_Client))
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_Widgets = ACC;
#endif

#if (defined(BUILD_Widgets) && defined(BUILD_Client))
template <>
size_t UniqueComponent::component_index<Engine::Widgets::WidgetManager>() { return CollectorBaseIndex_MainWindowComponentBase_Widgets + 0; }
#endif

#if (defined(BUILD_Widgets) && defined(BUILD_Client))
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_Widgets + 1
#endif

#if (defined(BUILD_VulkanTools) && defined(BUILD_Client))
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_VulkanTools = ACC;
#endif

#if (defined(BUILD_VulkanTools) && defined(BUILD_Client))
template <>
size_t UniqueComponent::component_index<Engine::Tools::VulkanImRoot>() { return CollectorBaseIndex_MainWindowComponentBase_VulkanTools + 0; }
#endif

#if (defined(BUILD_VulkanTools) && defined(BUILD_Client))
template <>
size_t UniqueComponent::component_index<Engine::Tools::ClientImRoot>() { return CollectorBaseIndex_MainWindowComponentBase_VulkanTools + 0; }
#endif

#if (defined(BUILD_VulkanTools) && defined(BUILD_Client))
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_VulkanTools + 1
#endif

#if (defined(BUILD_Client))

#    undef ACC

#endif

#if (defined(BUILD_App))
template <>
std::vector<Engine::App::GlobalAPIRegistry::F> Engine::App::GlobalAPIRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_Scene) && defined(BUILD_App))
		UniqueComponent::createComponent<Engine::Serialize::NoParent<Engine::Scene::SceneManager>>,
#endif

#if (defined(BUILD_App))

	}; 
}
#endif

#if (defined(BUILD_App))

#    define ACC 0

#endif

#if (defined(BUILD_Scene) && defined(BUILD_App))
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Scene = ACC;
#endif

#if (defined(BUILD_Scene) && defined(BUILD_App))
template <>
size_t UniqueComponent::component_index<Engine::Serialize::NoParent<Engine::Scene::SceneManager>>() { return CollectorBaseIndex_GlobalAPIBase_Scene + 0; }
#endif

#if (defined(BUILD_Scene) && defined(BUILD_App))
template <>
size_t UniqueComponent::component_index<Engine::Scene::SceneManager>() { return CollectorBaseIndex_GlobalAPIBase_Scene + 0; }
#endif

#if (defined(BUILD_Scene) && defined(BUILD_App))
#        undef ACC
#        define ACC CollectorBaseIndex_GlobalAPIBase_Scene + 1
#endif

#if (defined(BUILD_App))

#    undef ACC

#endif

#if (defined(BUILD_UI))
template <>
std::vector<Engine::Input::HandlerRegistry::F> Engine::Input::HandlerRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
		UniqueComponent::createComponent<ClickBrick::MainMenuHandler>,
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
		UniqueComponent::createComponent<ClickBrick::GameHandler>,
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
		UniqueComponent::createComponent<ClickBrick::GameManager>,
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
		UniqueComponent::createComponent<ClickBrick::GameOverHandler>,
#endif

#if (defined(BUILD_UI))

	}; 
}
#endif

#if (defined(BUILD_UI))

#    define ACC 0

#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
constexpr size_t CollectorBaseIndex_HandlerBase_ClickBrick = ACC;
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
template <>
size_t UniqueComponent::component_index<ClickBrick::MainMenuHandler>() { return CollectorBaseIndex_HandlerBase_ClickBrick + 0; }
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
template <>
size_t UniqueComponent::component_index<ClickBrick::GameHandler>() { return CollectorBaseIndex_HandlerBase_ClickBrick + 1; }
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
template <>
size_t UniqueComponent::component_index<ClickBrick::GameManager>() { return CollectorBaseIndex_HandlerBase_ClickBrick + 2; }
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
template <>
size_t UniqueComponent::component_index<ClickBrick::GameOverHandler>() { return CollectorBaseIndex_HandlerBase_ClickBrick + 3; }
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_UI))
#        undef ACC
#        define ACC CollectorBaseIndex_HandlerBase_ClickBrick + 4
#endif

#if (defined(BUILD_UI))

#    undef ACC

#endif

#if (defined(BUILD_Tools))
template <>
std::vector<Engine::Tools::ToolsRegistry::F> Engine::Tools::ToolsRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_ClientNodesTools) && defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::NodeRendererTester>,
#endif

#if (defined(BUILD_Tools) && defined(BUILD_ClientTools))
		UniqueComponent::createComponent<Engine::Tools::GamepadViewer>,
#endif

#if (defined(BUILD_Tools) && defined(BUILD_ClientTools))
		UniqueComponent::createComponent<Engine::Tools::ProjectManager>,
#endif

#if (defined(BUILD_DebuggerTools) && defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::DebuggerView>,
#endif

#if (defined(BUILD_MadgineLauncherTools) && defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::LauncherTool>,
#endif

#if (defined(BUILD_NodeGraphTools) && defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::NodeGraphEditor>,
#endif

#if (defined(BUILD_Python3Tools) && defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::Python3ImmediateWindow>,
#endif

#if (defined(BUILD_ResourcesTools) && defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::ResourcesToolConfig>,
#endif

#if (defined(BUILD_SceneRendererTools) && defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::SceneEditor>,
#endif

#if (defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::FileBrowser>,
#endif

#if (defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::FunctionTool>,
#endif

#if (defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::Inspector>,
#endif

#if (defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::LogViewer>,
#endif

#if (defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::Metrics>,
#endif

#if (defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::Profiler>,
#endif

#if (defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::ImGuiDemo>,
#endif

#if (defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::TestTool>,
#endif

#if (defined(BUILD_VulkanTools) && defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::VulkanToolConfig>,
#endif

#if (defined(BUILD_WidgetsTools) && defined(BUILD_Tools))
		UniqueComponent::createComponent<Engine::Tools::GuiEditor>,
#endif

#if (defined(BUILD_Tools))

	}; 
}
#endif

#if (defined(BUILD_Tools))

#    define ACC 0

#endif

#if (defined(BUILD_ClientNodesTools) && defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_ClientNodesTools = ACC;
#endif

#if (defined(BUILD_ClientNodesTools) && defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::NodeRendererTester>() { return CollectorBaseIndex_ToolBase_ClientNodesTools + 0; }
#endif

#if (defined(BUILD_ClientNodesTools) && defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_ClientNodesTools + 1
#endif

#if (defined(BUILD_Tools) && defined(BUILD_ClientTools))
constexpr size_t CollectorBaseIndex_ToolBase_ClientTools = ACC;
#endif

#if (defined(BUILD_Tools) && defined(BUILD_ClientTools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::GamepadViewer>() { return CollectorBaseIndex_ToolBase_ClientTools + 0; }
#endif

#if (defined(BUILD_Tools) && defined(BUILD_ClientTools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::ProjectManager>() { return CollectorBaseIndex_ToolBase_ClientTools + 1; }
#endif

#if (defined(BUILD_Tools) && defined(BUILD_ClientTools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_ClientTools + 2
#endif

#if (defined(BUILD_DebuggerTools) && defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_DebuggerTools = ACC;
#endif

#if (defined(BUILD_DebuggerTools) && defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::DebuggerView>() { return CollectorBaseIndex_ToolBase_DebuggerTools + 0; }
#endif

#if (defined(BUILD_DebuggerTools) && defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_DebuggerTools + 1
#endif

#if (defined(BUILD_MadgineLauncherTools) && defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_MadgineLauncherTools = ACC;
#endif

#if (defined(BUILD_MadgineLauncherTools) && defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::LauncherTool>() { return CollectorBaseIndex_ToolBase_MadgineLauncherTools + 0; }
#endif

#if (defined(BUILD_MadgineLauncherTools) && defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_MadgineLauncherTools + 1
#endif

#if (defined(BUILD_NodeGraphTools) && defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_NodeGraphTools = ACC;
#endif

#if (defined(BUILD_NodeGraphTools) && defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::NodeGraphEditor>() { return CollectorBaseIndex_ToolBase_NodeGraphTools + 0; }
#endif

#if (defined(BUILD_NodeGraphTools) && defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_NodeGraphTools + 1
#endif

#if (defined(BUILD_Python3Tools) && defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_Python3Tools = ACC;
#endif

#if (defined(BUILD_Python3Tools) && defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::Python3ImmediateWindow>() { return CollectorBaseIndex_ToolBase_Python3Tools + 0; }
#endif

#if (defined(BUILD_Python3Tools) && defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_Python3Tools + 1
#endif

#if (defined(BUILD_ResourcesTools) && defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_ResourcesTools = ACC;
#endif

#if (defined(BUILD_ResourcesTools) && defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::ResourcesToolConfig>() { return CollectorBaseIndex_ToolBase_ResourcesTools + 0; }
#endif

#if (defined(BUILD_ResourcesTools) && defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_ResourcesTools + 1
#endif

#if (defined(BUILD_SceneRendererTools) && defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_SceneRendererTools = ACC;
#endif

#if (defined(BUILD_SceneRendererTools) && defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::SceneEditor>() { return CollectorBaseIndex_ToolBase_SceneRendererTools + 0; }
#endif

#if (defined(BUILD_SceneRendererTools) && defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_SceneRendererTools + 1
#endif

#if (defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_Tools = ACC;
#endif

#if (defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::FileBrowser>() { return CollectorBaseIndex_ToolBase_Tools + 0; }
#endif

#if (defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::FunctionTool>() { return CollectorBaseIndex_ToolBase_Tools + 1; }
#endif

#if (defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::Inspector>() { return CollectorBaseIndex_ToolBase_Tools + 2; }
#endif

#if (defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::LogViewer>() { return CollectorBaseIndex_ToolBase_Tools + 3; }
#endif

#if (defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::Metrics>() { return CollectorBaseIndex_ToolBase_Tools + 4; }
#endif

#if (defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::Profiler>() { return CollectorBaseIndex_ToolBase_Tools + 5; }
#endif

#if (defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::ImGuiDemo>() { return CollectorBaseIndex_ToolBase_Tools + 6; }
#endif

#if (defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::TestTool>() { return CollectorBaseIndex_ToolBase_Tools + 7; }
#endif

#if (defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_Tools + 8
#endif

#if (defined(BUILD_VulkanTools) && defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_VulkanTools = ACC;
#endif

#if (defined(BUILD_VulkanTools) && defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::VulkanToolConfig>() { return CollectorBaseIndex_ToolBase_VulkanTools + 0; }
#endif

#if (defined(BUILD_VulkanTools) && defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_VulkanTools + 1
#endif

#if (defined(BUILD_WidgetsTools) && defined(BUILD_Tools))
constexpr size_t CollectorBaseIndex_ToolBase_WidgetsTools = ACC;
#endif

#if (defined(BUILD_WidgetsTools) && defined(BUILD_Tools))
template <>
size_t UniqueComponent::component_index<Engine::Tools::GuiEditor>() { return CollectorBaseIndex_ToolBase_WidgetsTools + 0; }
#endif

#if (defined(BUILD_WidgetsTools) && defined(BUILD_Tools))
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_WidgetsTools + 1
#endif

#if (defined(BUILD_Tools))

#    undef ACC

#endif

#if (defined(BUILD_NodeGraph))
template <>
std::vector<Engine::NodeGraph::NodeBaseRegistry::F> Engine::NodeGraph::NodeBaseRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<Engine::Render::GPUBufferNode>,
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<Engine::Render::FrameBufferNode>,
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<Engine::Render::MeshRendererNode>,
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<Engine::Render::RasterizerNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<Engine::NodeGraph::FunctionNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<Engine::NodeGraph::PumpNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<Engine::NodeGraph::TestNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<ForEachNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<LetValueNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<AddNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<LogNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<JustNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<SequenceNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<VariableNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<WriteVarNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<ReadVarNode>,
#endif

#if (defined(BUILD_NodeGraph))
		UniqueComponent::createComponent<AssignNode>,
#endif

#if (defined(BUILD_NodeGraph))

	}; 
}
#endif

#if (defined(BUILD_NodeGraph))

#    define ACC 0

#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
constexpr size_t CollectorBaseIndex_NodeBase_ClientNodes = ACC;
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<Engine::Render::GPUBufferNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 0; }
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<Engine::Render::FrameBufferNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 1; }
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<Engine::Render::MeshRendererNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 2; }
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<Engine::Render::RasterizerNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 3; }
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
#        undef ACC
#        define ACC CollectorBaseIndex_NodeBase_ClientNodes + 4
#endif

#if (defined(BUILD_NodeGraph))
constexpr size_t CollectorBaseIndex_NodeBase_NodeGraph = ACC;
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::FunctionNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 0; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::PumpNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 1; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::TestNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 2; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<ForEachNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 3; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<LetValueNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 4; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<AddNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 5; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<LogNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 6; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<JustNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 7; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<SequenceNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 8; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<VariableNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 9; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<WriteVarNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 10; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<ReadVarNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 11; }
#endif

#if (defined(BUILD_NodeGraph))
template <>
size_t UniqueComponent::component_index<AssignNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 12; }
#endif

#if (defined(BUILD_NodeGraph))
#        undef ACC
#        define ACC CollectorBaseIndex_NodeBase_NodeGraph + 13
#endif

#if (defined(BUILD_NodeGraph))

#    undef ACC

#endif

#if (defined(BUILD_NodeGraph))
template <>
const std::map<std::string_view, IndexType<uint32_t>> &Engine::NodeGraph::NodeRegistry::sComponentsByName()
{
    static std::map<std::string_view, IndexType<uint32_t>> mapping {
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
		{"GPUBufferNode", CollectorBaseIndex_NodeBase_ClientNodes + 0},
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
		{"FrameBufferNode", CollectorBaseIndex_NodeBase_ClientNodes + 1},
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
		{"MeshRendererNode", CollectorBaseIndex_NodeBase_ClientNodes + 2},
#endif

#if (defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph))
		{"RasterizerNode", CollectorBaseIndex_NodeBase_ClientNodes + 3},
#endif

#if (defined(BUILD_NodeGraph))
		{"FunctionNode", CollectorBaseIndex_NodeBase_NodeGraph + 0},
#endif

#if (defined(BUILD_NodeGraph))
		{"PumpNode", CollectorBaseIndex_NodeBase_NodeGraph + 1},
#endif

#if (defined(BUILD_NodeGraph))
		{"TestNode", CollectorBaseIndex_NodeBase_NodeGraph + 2},
#endif

#if (defined(BUILD_NodeGraph))
		{"ForEach", CollectorBaseIndex_NodeBase_NodeGraph + 3},
#endif

#if (defined(BUILD_NodeGraph))
		{"LetValue", CollectorBaseIndex_NodeBase_NodeGraph + 4},
#endif

#if (defined(BUILD_NodeGraph))
		{"Add", CollectorBaseIndex_NodeBase_NodeGraph + 5},
#endif

#if (defined(BUILD_NodeGraph))
		{"Log", CollectorBaseIndex_NodeBase_NodeGraph + 6},
#endif

#if (defined(BUILD_NodeGraph))
		{"Just", CollectorBaseIndex_NodeBase_NodeGraph + 7},
#endif

#if (defined(BUILD_NodeGraph))
		{"Sequence", CollectorBaseIndex_NodeBase_NodeGraph + 8},
#endif

#if (defined(BUILD_NodeGraph))
		{"Variable", CollectorBaseIndex_NodeBase_NodeGraph + 9},
#endif

#if (defined(BUILD_NodeGraph))
		{"WriteVar", CollectorBaseIndex_NodeBase_NodeGraph + 10},
#endif

#if (defined(BUILD_NodeGraph))
		{"ReadVar", CollectorBaseIndex_NodeBase_NodeGraph + 11},
#endif

#if (defined(BUILD_NodeGraph))
		{"Assign", CollectorBaseIndex_NodeBase_NodeGraph + 12},
#endif

#if (defined(BUILD_NodeGraph))

	}; 
    return mapping;
}
#endif

#if (defined(BUILD_Scene))
template <>
std::vector<Engine::Scene::SceneComponentRegistry::F> Engine::Scene::SceneComponentRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_Scene))

	}; 
}
#endif

#if (defined(BUILD_Scene))

#    define ACC 0

#endif

#if (defined(BUILD_Scene))

#    undef ACC

#endif

#if (defined(BUILD_Scene))
template <>
std::vector<Engine::Scene::Entity::EntityComponentBaseRegistry::F> Engine::Scene::Entity::EntityComponentBaseRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::Animation>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::Material>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::Mesh>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::PointLight>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::Skeleton>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::Transform>,
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
		UniqueComponent::createComponent<ClickBrick::Scene::Brick>,
#endif

#if (defined(BUILD_Scene))

	}; 
}
#endif

#if (defined(BUILD_Scene))

#    define ACC 0

#endif

#if (defined(BUILD_Scene))
constexpr size_t CollectorBaseIndex_EntityComponentBase_Scene = ACC;
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Animation>() { return CollectorBaseIndex_EntityComponentBase_Scene + 0; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Material>() { return CollectorBaseIndex_EntityComponentBase_Scene + 1; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Mesh>() { return CollectorBaseIndex_EntityComponentBase_Scene + 2; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::PointLight>() { return CollectorBaseIndex_EntityComponentBase_Scene + 3; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Skeleton>() { return CollectorBaseIndex_EntityComponentBase_Scene + 4; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Transform>() { return CollectorBaseIndex_EntityComponentBase_Scene + 5; }
#endif

#if (defined(BUILD_Scene))
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentBase_Scene + 6
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
constexpr size_t CollectorBaseIndex_EntityComponentBase_ClickBrick = ACC;
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<ClickBrick::Scene::Brick>() { return CollectorBaseIndex_EntityComponentBase_ClickBrick + 0; }
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentBase_ClickBrick + 1
#endif

#if (defined(BUILD_Scene))

#    undef ACC

#endif

#if (defined(BUILD_Scene))
template <>
const std::map<std::string_view, IndexType<uint32_t>> &Engine::Scene::Entity::EntityComponentRegistry::sComponentsByName()
{
    static std::map<std::string_view, IndexType<uint32_t>> mapping {
#endif

#if (defined(BUILD_Scene))
		{"Animation", CollectorBaseIndex_EntityComponentBase_Scene + 0},
#endif

#if (defined(BUILD_Scene))
		{"Material", CollectorBaseIndex_EntityComponentBase_Scene + 1},
#endif

#if (defined(BUILD_Scene))
		{"Mesh", CollectorBaseIndex_EntityComponentBase_Scene + 2},
#endif

#if (defined(BUILD_Scene))
		{"PointLight", CollectorBaseIndex_EntityComponentBase_Scene + 3},
#endif

#if (defined(BUILD_Scene))
		{"Skeleton", CollectorBaseIndex_EntityComponentBase_Scene + 4},
#endif

#if (defined(BUILD_Scene))
		{"Transform", CollectorBaseIndex_EntityComponentBase_Scene + 5},
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
		{"Brick", CollectorBaseIndex_EntityComponentBase_ClickBrick + 0},
#endif

#if (defined(BUILD_Scene))

	}; 
    return mapping;
}
#endif

#if (defined(BUILD_Scene))
template <>
std::vector<Engine::Scene::Entity::EntityComponentListRegistry::F> Engine::Scene::Entity::EntityComponentListRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Material>>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Mesh>>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::PointLight>>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Skeleton>>,
#endif

#if (defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Transform>>,
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<ClickBrick::Scene::Brick>>,
#endif

#if (defined(BUILD_Scene))

	}; 
}
#endif

#if (defined(BUILD_Scene))

#    define ACC 0

#endif

#if (defined(BUILD_Scene))
constexpr size_t CollectorBaseIndex_EntityComponentListBase_Scene = ACC;
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 0; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Material>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 1; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Mesh>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 2; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::PointLight>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 3; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Skeleton>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 4; }
#endif

#if (defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Transform>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 5; }
#endif

#if (defined(BUILD_Scene))
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_Scene + 6
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
constexpr size_t CollectorBaseIndex_EntityComponentListBase_ClickBrick = ACC;
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<ClickBrick::Scene::Brick>>() { return CollectorBaseIndex_EntityComponentListBase_ClickBrick + 0; }
#endif

#if (defined(BUILD_ClickBrick) && defined(BUILD_Scene))
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_ClickBrick + 1
#endif

#if (defined(BUILD_Scene))

#    undef ACC

#endif

#if (defined(BUILD_TestShared))
template <>
std::vector<Test::TestRegistry::F> Test::TestRegistry::sComponents()
{
	return {
#endif

#if (defined(BUILD_TestShared))

	}; 
}
#endif

#if (defined(BUILD_TestShared))

#    define ACC 0

#endif

#if (defined(BUILD_TestShared))

#    undef ACC

#endif

}

