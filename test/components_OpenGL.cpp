#include "Modules/moduleslib.h"
#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentregistry.h"
#if defined(BUILD_ClientNodes)
#include "Madgine/clientnodeslib.h"
#endif
#if defined(BUILD_MeshLoader)
#include "Madgine/meshloaderlib.h"
#endif
#if defined(BUILD_Widgets)
#include "Madgine/widgetslib.h"
#endif
#if defined(BUILD_FontLoader)
#include "Madgine/fontloaderlib.h"
#endif
#if defined(BUILD_Root)
#include "Madgine/rootlib.h"
#endif
#if defined(BUILD_Client)
#include "Madgine/clientlib.h"
#endif
#if defined(BUILD_UI)
#include "Madgine/uilib.h"
#endif
#if defined(BUILD_App)
#include "Madgine/applib.h"
#endif
#if defined(BUILD_Scene)
#include "Madgine/scenelib.h"
#endif
#if defined(BUILD_OpenGL)
#include "OpenGL/opengllib.h"
#endif
#if defined(BUILD_NodeGraph)
#include "Madgine/nodegraphlib.h"
#endif
#if defined(BUILD_AnimationLoader)
#include "Madgine/animationloaderlib.h"
#endif
#if defined(BUILD_SkeletonLoader)
#include "Madgine/skeletonloaderlib.h"
#endif
#if defined(BUILD_ClickBrick)
#include "clickbricklib.h"
#endif
#if defined(BUILD_ImageLoader)
#include "Madgine/imageloaderlib.h"
#endif
#if defined(BUILD_SceneRenderer)
#include "Madgine/scenerendererlib.h"
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
#include "Madgine/meshloader/meshloader.h"
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
#include "Madgine/fontloader/fontloader.h"
#endif
#if defined(BUILD_Root)
#include "Madgine/root/rootcomponentcollector.h"
#endif
#if defined(BUILD_Root) && defined(BUILD_Resources)
#include "Madgine/resources/resourcemanager.h"
#endif
#if defined(BUILD_Client)
#include "Madgine/render/rendercontextcollector.h"
#include "Madgine/window/mainwindowcomponentcollector.h"
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
#include "Madgine/widgets/widgetmanager.h"
#endif
#if defined(BUILD_UI)
#include "Madgine/input/handlercollector.h"
#endif
#if defined(BUILD_App)
#include "Madgine/app/globalapicollector.h"
#endif
#if defined(BUILD_Scene)
#include "Madgine/scene/entity/components/animation.h"
#include "Madgine/scene/entity/components/material.h"
#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/pointlight.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entitycomponentcollector.h"
#include "Madgine/scene/scenecomponentcollector.h"
#endif
#if defined(BUILD_Scene) && defined(BUILD_App)
#include "Madgine/scene/scenemanager.h"
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
#include "OpenGL/openglmeshloader.h"
#include "OpenGL/openglpipelineloader.h"
#include "OpenGL/openglshaderloader.h"
#include "OpenGL/opengltextureloader.h"
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
#include "OpenGL/openglrendercontext.h"
#endif
#if defined(BUILD_NodeGraph)
#include "Madgine/nodegraph/nodecollector.h"
#include "Madgine/nodegraph/nodes/arithmetical/additionnode.h"
#include "Madgine/nodegraph/nodes/arithmetical/multiplicationnode.h"
#include "Madgine/nodegraph/nodes/arithmetical/vector3to4node.h"
#include "Madgine/nodegraph/nodes/functionnode.h"
#include "Madgine/nodegraph/nodes/pumpnode.h"
#include "Madgine/nodegraph/nodes/testnode.h"
#include "Madgine/nodegraph/nodes/variablenode.h"
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Resources)
#include "Madgine/nodegraph/nodegraphloader.h"
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
#include "Madgine/client/nodes/framebuffernode.h"
#include "Madgine/client/nodes/gpubuffernode.h"
#include "Madgine/client/nodes/meshrenderernode.h"
#include "Madgine/client/nodes/rasterizernode.h"
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
#include "Madgine/animationloader/animationloader.h"
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
#include "Madgine/skeletonloader/skeletonloader.h"
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
#include "gamehandler.h"
#include "gamemanager.h"
#include "gameoverhandler.h"
#include "mainmenuhandler.h"
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
#include "brick.h"
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
#include "Madgine/imageloader/imageloader.h"
#endif
#if defined(BUILD_SceneRenderer) && defined(BUILD_Client)
#include "Madgine/render/scenemainwindowcomponent.h"
#endif


namespace Engine{
#if defined(BUILD_Root)
template <>
std::vector<Engine::Root::RootComponentRegistry::F> Engine::Root::RootComponentRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Root) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Resources::ResourceManager>,
#endif
#if defined(BUILD_Root)

	}; 
}
#endif
#if defined(BUILD_Root)

#    define ACC 0

#endif
#if defined(BUILD_Root) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_RootComponentBase_Resources = ACC;
#endif
#if defined(BUILD_Root) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Resources::ResourceManager>() { return CollectorBaseIndex_RootComponentBase_Resources + 0; }
#endif
#if defined(BUILD_Root) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_RootComponentBase_Resources + 1
#endif
#if defined(BUILD_Root)

#    undef ACC

#endif
#if defined(BUILD_Resources)
template <>
std::vector<Engine::Resources::ResourceLoaderRegistry::F> Engine::Resources::ResourceLoaderRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::NodeGraph::NodeGraphLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Render::OpenGLMeshLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Render::OpenGLPipelineLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Render::OpenGLShaderLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Render::OpenGLTextureLoader>,
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Render::AnimationLoader>,
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Render::FontLoader>,
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Resources::ImageLoader>,
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Render::MeshLoader>,
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
		UniqueComponent::createComponent<Engine::Render::SkeletonLoader>,
#endif
#if defined(BUILD_Resources)

	}; 
}
#endif
#if defined(BUILD_Resources)

#    define ACC 0

#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_NodeGraph = ACC;
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::NodeGraphLoader>() { return CollectorBaseIndex_ResourceLoaderBase_NodeGraph + 0; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_NodeGraph + 1
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_OpenGL = ACC;
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::OpenGLMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 0; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::GPUMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 0; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::OpenGLPipelineLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::PipelineLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::OpenGLShaderLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 2; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::OpenGLTextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 3; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::TextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 3; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_OpenGL + 4
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_AnimationLoader = ACC;
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::AnimationLoader>() { return CollectorBaseIndex_ResourceLoaderBase_AnimationLoader + 0; }
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_AnimationLoader + 1
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_FontLoader = ACC;
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::FontLoader>() { return CollectorBaseIndex_ResourceLoaderBase_FontLoader + 0; }
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_FontLoader + 1
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_ImageLoader = ACC;
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Resources::ImageLoader>() { return CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 0; }
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 1
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_MeshLoader = ACC;
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::MeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_MeshLoader + 0; }
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_MeshLoader + 1
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader = ACC;
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
template <>
size_t UniqueComponent::component_index<Engine::Render::SkeletonLoader>() { return CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader + 0; }
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader + 1
#endif
#if defined(BUILD_Resources)

#    undef ACC

#endif
#if defined(BUILD_Client)
template <>
std::vector<Engine::Render::RenderContextRegistry::F> Engine::Render::RenderContextRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
		UniqueComponent::createComponent<Engine::Render::OpenGLRenderContext>,
#endif
#if defined(BUILD_Client)

	}; 
}
#endif
#if defined(BUILD_Client)

#    define ACC 0

#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
constexpr size_t CollectorBaseIndex_RenderContext_OpenGL = ACC;
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
template <>
size_t UniqueComponent::component_index<Engine::Render::OpenGLRenderContext>() { return CollectorBaseIndex_RenderContext_OpenGL + 0; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Client)
#        undef ACC
#        define ACC CollectorBaseIndex_RenderContext_OpenGL + 1
#endif
#if defined(BUILD_Client)

#    undef ACC

#endif
#if defined(BUILD_Client)
template <>
std::vector<Engine::Window::MainWindowComponentRegistry::F> Engine::Window::MainWindowComponentRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_SceneRenderer) && defined(BUILD_Client)
		UniqueComponent::createComponent<Engine::Render::SceneMainWindowComponent>,
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
		UniqueComponent::createComponent<Engine::Widgets::WidgetManager>,
#endif
#if defined(BUILD_Client)

	}; 
}
#endif
#if defined(BUILD_Client)

#    define ACC 0

#endif
#if defined(BUILD_SceneRenderer) && defined(BUILD_Client)
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_SceneRenderer = ACC;
#endif
#if defined(BUILD_SceneRenderer) && defined(BUILD_Client)
template <>
size_t UniqueComponent::component_index<Engine::Render::SceneMainWindowComponent>() { return CollectorBaseIndex_MainWindowComponentBase_SceneRenderer + 0; }
#endif
#if defined(BUILD_SceneRenderer) && defined(BUILD_Client)
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_SceneRenderer + 1
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_Widgets = ACC;
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
template <>
size_t UniqueComponent::component_index<Engine::Widgets::WidgetManager>() { return CollectorBaseIndex_MainWindowComponentBase_Widgets + 0; }
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_Widgets + 1
#endif
#if defined(BUILD_Client)

#    undef ACC

#endif
#if defined(BUILD_App)
template <>
std::vector<Engine::App::GlobalAPIRegistry::F> Engine::App::GlobalAPIRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Scene) && defined(BUILD_App)
		UniqueComponent::createComponent<Engine::Serialize::NoParent<Engine::Scene::SceneManager>>,
#endif
#if defined(BUILD_App)

	}; 
}
#endif
#if defined(BUILD_App)

#    define ACC 0

#endif
#if defined(BUILD_Scene) && defined(BUILD_App)
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Scene = ACC;
#endif
#if defined(BUILD_Scene) && defined(BUILD_App)
template <>
size_t UniqueComponent::component_index<Engine::Serialize::NoParent<Engine::Scene::SceneManager>>() { return CollectorBaseIndex_GlobalAPIBase_Scene + 0; }
#endif
#if defined(BUILD_Scene) && defined(BUILD_App)
template <>
size_t UniqueComponent::component_index<Engine::Scene::SceneManager>() { return CollectorBaseIndex_GlobalAPIBase_Scene + 0; }
#endif
#if defined(BUILD_Scene) && defined(BUILD_App)
#        undef ACC
#        define ACC CollectorBaseIndex_GlobalAPIBase_Scene + 1
#endif
#if defined(BUILD_App)

#    undef ACC

#endif
#if defined(BUILD_UI)
template <>
std::vector<Engine::Input::HandlerRegistry::F> Engine::Input::HandlerRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
		UniqueComponent::createComponent<ClickBrick::MainMenuHandler>,
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
		UniqueComponent::createComponent<ClickBrick::GameHandler>,
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
		UniqueComponent::createComponent<ClickBrick::GameManager>,
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
		UniqueComponent::createComponent<ClickBrick::GameOverHandler>,
#endif
#if defined(BUILD_UI)

	}; 
}
#endif
#if defined(BUILD_UI)

#    define ACC 0

#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
constexpr size_t CollectorBaseIndex_HandlerBase_ClickBrick = ACC;
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
template <>
size_t UniqueComponent::component_index<ClickBrick::MainMenuHandler>() { return CollectorBaseIndex_HandlerBase_ClickBrick + 0; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
template <>
size_t UniqueComponent::component_index<ClickBrick::GameHandler>() { return CollectorBaseIndex_HandlerBase_ClickBrick + 1; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
template <>
size_t UniqueComponent::component_index<ClickBrick::GameManager>() { return CollectorBaseIndex_HandlerBase_ClickBrick + 2; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
template <>
size_t UniqueComponent::component_index<ClickBrick::GameOverHandler>() { return CollectorBaseIndex_HandlerBase_ClickBrick + 3; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
#        undef ACC
#        define ACC CollectorBaseIndex_HandlerBase_ClickBrick + 4
#endif
#if defined(BUILD_UI)

#    undef ACC

#endif
#if defined(BUILD_NodeGraph)
template <>
std::vector<Engine::NodeGraph::NodeBaseRegistry::F> Engine::NodeGraph::NodeBaseRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		UniqueComponent::createComponent<Engine::Render::GPUBufferNode>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		UniqueComponent::createComponent<Engine::Render::FrameBufferNode>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		UniqueComponent::createComponent<Engine::Render::MeshRendererNode>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		UniqueComponent::createComponent<Engine::Render::RasterizerNode>,
#endif
#if defined(BUILD_NodeGraph)
		UniqueComponent::createComponent<Engine::NodeGraph::FunctionNode>,
#endif
#if defined(BUILD_NodeGraph)
		UniqueComponent::createComponent<Engine::NodeGraph::PumpNode>,
#endif
#if defined(BUILD_NodeGraph)
		UniqueComponent::createComponent<Engine::NodeGraph::TestNode>,
#endif
#if defined(BUILD_NodeGraph)
		UniqueComponent::createComponent<Engine::NodeGraph::VariableNode>,
#endif
#if defined(BUILD_NodeGraph)
		UniqueComponent::createComponent<Engine::NodeGraph::AdditionNode>,
#endif
#if defined(BUILD_NodeGraph)
		UniqueComponent::createComponent<Engine::NodeGraph::MultiplicationNode>,
#endif
#if defined(BUILD_NodeGraph)
		UniqueComponent::createComponent<Engine::NodeGraph::Vector3to4Node>,
#endif
#if defined(BUILD_NodeGraph)

	}; 
}
#endif
#if defined(BUILD_NodeGraph)

#    define ACC 0

#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
constexpr size_t CollectorBaseIndex_NodeBase_ClientNodes = ACC;
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
template <>
size_t UniqueComponent::component_index<Engine::Render::GPUBufferNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 0; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
template <>
size_t UniqueComponent::component_index<Engine::Render::FrameBufferNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 1; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
template <>
size_t UniqueComponent::component_index<Engine::Render::MeshRendererNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 2; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
template <>
size_t UniqueComponent::component_index<Engine::Render::RasterizerNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 3; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
#        undef ACC
#        define ACC CollectorBaseIndex_NodeBase_ClientNodes + 4
#endif
#if defined(BUILD_NodeGraph)
constexpr size_t CollectorBaseIndex_NodeBase_NodeGraph = ACC;
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::FunctionNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 0; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::PumpNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 1; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::TestNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 2; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::VariableNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 3; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::AdditionNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 4; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::MultiplicationNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 5; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t UniqueComponent::component_index<Engine::NodeGraph::Vector3to4Node>() { return CollectorBaseIndex_NodeBase_NodeGraph + 6; }
#endif
#if defined(BUILD_NodeGraph)
#        undef ACC
#        define ACC CollectorBaseIndex_NodeBase_NodeGraph + 7
#endif
#if defined(BUILD_NodeGraph)

#    undef ACC

#endif
#if defined(BUILD_NodeGraph)
template <>
std::map<std::string_view, size_t> Engine::NodeGraph::NodeRegistry::sComponentsByName()
{
	return {
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		{"GPUBufferNode", CollectorBaseIndex_NodeBase_ClientNodes + 0},
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		{"FrameBufferNode", CollectorBaseIndex_NodeBase_ClientNodes + 1},
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		{"MeshRendererNode", CollectorBaseIndex_NodeBase_ClientNodes + 2},
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		{"RasterizerNode", CollectorBaseIndex_NodeBase_ClientNodes + 3},
#endif
#if defined(BUILD_NodeGraph)
		{"FunctionNode", CollectorBaseIndex_NodeBase_NodeGraph + 0},
#endif
#if defined(BUILD_NodeGraph)
		{"PumpNode", CollectorBaseIndex_NodeBase_NodeGraph + 1},
#endif
#if defined(BUILD_NodeGraph)
		{"TestNode", CollectorBaseIndex_NodeBase_NodeGraph + 2},
#endif
#if defined(BUILD_NodeGraph)
		{"VariableNode", CollectorBaseIndex_NodeBase_NodeGraph + 3},
#endif
#if defined(BUILD_NodeGraph)
		{"AdditionNode", CollectorBaseIndex_NodeBase_NodeGraph + 4},
#endif
#if defined(BUILD_NodeGraph)
		{"MultiplicationNode", CollectorBaseIndex_NodeBase_NodeGraph + 5},
#endif
#if defined(BUILD_NodeGraph)
		{"Vector3to4Node", CollectorBaseIndex_NodeBase_NodeGraph + 6},
#endif
#if defined(BUILD_NodeGraph)

	}; 
}
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
#endif
#if defined(BUILD_Scene)

#    define ACC 0

#endif
#if defined(BUILD_Scene)

#    undef ACC

#endif
#if defined(BUILD_Scene)
template <>
std::vector<Engine::Scene::Entity::EntityComponentBaseRegistry::F> Engine::Scene::Entity::EntityComponentBaseRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::Animation>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::Material>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::Mesh>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::PointLight>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::Skeleton>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::Transform>,
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
		UniqueComponent::createComponent<ClickBrick::Scene::Brick>,
#endif
#if defined(BUILD_Scene)

	}; 
}
#endif
#if defined(BUILD_Scene)

#    define ACC 0

#endif
#if defined(BUILD_Scene)
constexpr size_t CollectorBaseIndex_EntityComponentBase_Scene = ACC;
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Animation>() { return CollectorBaseIndex_EntityComponentBase_Scene + 0; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Material>() { return CollectorBaseIndex_EntityComponentBase_Scene + 1; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Mesh>() { return CollectorBaseIndex_EntityComponentBase_Scene + 2; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::PointLight>() { return CollectorBaseIndex_EntityComponentBase_Scene + 3; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Skeleton>() { return CollectorBaseIndex_EntityComponentBase_Scene + 4; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::Transform>() { return CollectorBaseIndex_EntityComponentBase_Scene + 5; }
#endif
#if defined(BUILD_Scene)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentBase_Scene + 6
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
constexpr size_t CollectorBaseIndex_EntityComponentBase_ClickBrick = ACC;
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
template <>
size_t UniqueComponent::component_index<ClickBrick::Scene::Brick>() { return CollectorBaseIndex_EntityComponentBase_ClickBrick + 0; }
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
std::map<std::string_view, size_t> Engine::Scene::Entity::EntityComponentRegistry::sComponentsByName()
{
	return {
#endif
#if defined(BUILD_Scene)
		{"Animation", CollectorBaseIndex_EntityComponentBase_Scene + 0},
#endif
#if defined(BUILD_Scene)
		{"Material", CollectorBaseIndex_EntityComponentBase_Scene + 1},
#endif
#if defined(BUILD_Scene)
		{"Mesh", CollectorBaseIndex_EntityComponentBase_Scene + 2},
#endif
#if defined(BUILD_Scene)
		{"PointLight", CollectorBaseIndex_EntityComponentBase_Scene + 3},
#endif
#if defined(BUILD_Scene)
		{"Skeleton", CollectorBaseIndex_EntityComponentBase_Scene + 4},
#endif
#if defined(BUILD_Scene)
		{"Transform", CollectorBaseIndex_EntityComponentBase_Scene + 5},
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
		{"Brick", CollectorBaseIndex_EntityComponentBase_ClickBrick + 0},
#endif
#if defined(BUILD_Scene)

	}; 
}
#endif
#if defined(BUILD_Scene)
template <>
std::vector<Engine::Scene::Entity::EntityComponentListRegistry::F> Engine::Scene::Entity::EntityComponentListRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Material>>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Mesh>>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::PointLight>>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Skeleton>>,
#endif
#if defined(BUILD_Scene)
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Transform>>,
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
		UniqueComponent::createComponent<Engine::Scene::Entity::EntityComponentList<ClickBrick::Scene::Brick>>,
#endif
#if defined(BUILD_Scene)

	}; 
}
#endif
#if defined(BUILD_Scene)

#    define ACC 0

#endif
#if defined(BUILD_Scene)
constexpr size_t CollectorBaseIndex_EntityComponentListBase_Scene = ACC;
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 0; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Material>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 1; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Mesh>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 2; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::PointLight>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 3; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Skeleton>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 4; }
#endif
#if defined(BUILD_Scene)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Transform>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 5; }
#endif
#if defined(BUILD_Scene)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_Scene + 6
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
constexpr size_t CollectorBaseIndex_EntityComponentListBase_ClickBrick = ACC;
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
template <>
size_t UniqueComponent::component_index<Engine::Scene::Entity::EntityComponentList<ClickBrick::Scene::Brick>>() { return CollectorBaseIndex_EntityComponentListBase_ClickBrick + 0; }
#endif
#if defined(BUILD_Scene) && defined(BUILD_ClickBrick)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_ClickBrick + 1
#endif
#if defined(BUILD_Scene)

#    undef ACC

#endif
#if defined(BUILD_TestShared)
template <>
std::vector<Test::TestRegistry::F> Test::TestRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibA)
		UniqueComponent::createComponent<LibAComponent>,
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibB)
		UniqueComponent::createComponent<LibBComponent>,
#endif
#if defined(BUILD_TestShared)

	}; 
}
#endif
#if defined(BUILD_TestShared)

#    define ACC 0

#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibA)
constexpr size_t CollectorBaseIndex_TestBase_LibA = ACC;
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibA)
template <>
size_t UniqueComponent::component_index<LibAComponent>() { return CollectorBaseIndex_TestBase_LibA + 0; }
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
size_t UniqueComponent::component_index<LibBComponent>() { return CollectorBaseIndex_TestBase_LibB + 0; }
#endif
#if defined(BUILD_TestShared) && defined(BUILD_LibB)
#        undef ACC
#        define ACC CollectorBaseIndex_TestBase_LibB + 1
#endif
#if defined(BUILD_TestShared)

#    undef ACC

#endif
}
