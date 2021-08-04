#include "Modules/moduleslib.h"
#include "Modules/uniquecomponent/uniquecomponentregistry.h"
#if defined(BUILD_Controls)
#include "Madgine/controlslib.h"
#endif
#if defined(BUILD_MeshLoader)
#include "meshloaderlib.h"
#endif
#if defined(BUILD_ClientNodes)
#include "Madgine/clientnodeslib.h"
#endif
#if defined(BUILD_PlayerControls)
#include "Madgine/playercontrolslib.h"
#endif
#if defined(BUILD_Client)
#include "Madgine/clientlib.h"
#endif
#if defined(BUILD_Base)
#include "Madgine/baselib.h"
#endif
#if defined(BUILD_ControlsTools)
#include "Madgine_Tools/controlstoolslib.h"
#endif
#if defined(BUILD_ClientControls)
#include "Madgine/clientcontrolslib.h"
#endif
#if defined(BUILD_Widgets)
#include "Madgine/widgetslib.h"
#endif
#if defined(BUILD_OpenGLTools)
#include "OpenGL_Tools/opengltoolslib.h"
#endif
#if defined(BUILD_UI)
#include "Madgine/uilib.h"
#endif
#if defined(BUILD_FontLoader)
#include "fontloaderlib.h"
#endif
#if defined(BUILD_Python3Nodes)
#include "../nodes/Madgine/python3nodeslib.h"
#endif
#if defined(BUILD_ImageLoader)
#include "imageloaderlib.h"
#endif
#if defined(BUILD_SkeletonLoader)
#include "skeletonloaderlib.h"
#endif
#if defined(BUILD_AnimationLoader)
#include "animationloaderlib.h"
#endif
#if defined(BUILD_OpenGL)
#include "OpenGL/opengllib.h"
#endif
#if defined(BUILD_Bullet)
#include "Madgine/bulletlib.h"
#endif
#if defined(BUILD_Python3)
#include "Python3/python3lib.h"
#endif
#if defined(BUILD_ClickBrick)
#include "clickbricklib.h"
#endif
#if defined(BUILD_UlamTools)
#include "ulamtoolslib.h"
#endif
#if defined(BUILD_Scene)
#include "Madgine/scenelib.h"
#endif
#if defined(BUILD_NodeGraph)
#include "Madgine/nodegraphlib.h"
#endif
#if defined(BUILD_OpenALPlugin)
#include "OpenAL/openallib.h"
#endif
#if defined(BUILD_WidgetsTools)
#include "Madgine_Tools/widgetstoolslib.h"
#endif
#if defined(BUILD_SceneRendererTools)
#include "Madgine_Tools/scenerenderertoolslib.h"
#endif
#if defined(BUILD_ClientNodesTools)
#include "../nodes_tools/Madgine/clientnodestoolslib.h"
#endif
#if defined(BUILD_BulletTools)
#include "Madgine_Tools/bullettoolslib.h"
#endif
#if defined(BUILD_Python3Tools)
#include "Madgine_Tools/Python3/python3toolslib.h"
#endif
#if defined(BUILD_NodeGraphTools)
#include "Madgine_Tools/nodegraphtoolslib.h"
#endif
#if defined(BUILD_Tools)
#include "toolslib.h"
#endif
#if defined(BUILD_AudioLoader)
#include "audioloaderlib.h"
#endif

#if defined(BUILD_Resources)
#include "Madgine/resources/resourceloadercollector.h"
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
#include "meshloader.h"
#endif
#if defined(BUILD_Client)
#include "Madgine/render/rendercontextcollector.h"
#include "Madgine/window/mainwindowcomponentcollector.h"
#endif
#if defined(BUILD_Base)
#include "Madgine/app/globalapicollector.h"
#endif
#if defined(BUILD_Controls) && defined(BUILD_Base)
#include "Madgine/controls/controlsmanager.h"
#endif
#if defined(BUILD_Client) && defined(BUILD_ClientControls)
#include "Madgine/controls/clientcontrolsmanager.h"
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
#include "Madgine/widgets/widgetmanager.h"
#endif
#if defined(BUILD_OpenGLTools) && defined(BUILD_Client)
#include "OpenGL_Tools/imgui/openglimroot.h"
#endif
#if defined(BUILD_UI)
#include "Madgine/ui/handlercollector.h"
#endif
#if defined(BUILD_UI) && defined(BUILD_Client)
#include "Madgine/ui/uimanager.h"
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
#include "fontloader.h"
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
#include "imageloader.h"
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
#include "skeletonloader.h"
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
#include "animationloader.h"
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
#if defined(BUILD_Bullet) && defined(BUILD_Resources)
#include "Madgine/physics/collisionshapemanager.h"
#endif
#if defined(BUILD_Python3) && defined(BUILD_Resources)
#include "Python3/python3fileloader.h"
#endif
#if defined(BUILD_Python3) && defined(BUILD_Base)
#include "Python3/python3env.h"
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
#include "gamehandler.h"
#include "gamemanager.h"
#include "gameoverhandler.h"
#include "mainmenuhandler.h"
#endif
#if defined(BUILD_Scene)
#include "Madgine/scene/entity/components/animation.h"
#include "Madgine/scene/entity/components/mesh.h"
#include "Madgine/scene/entity/components/skeleton.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entitycomponentcollector.h"
#include "Madgine/scene/scenecomponentcollector.h"
#endif
#if defined(BUILD_Scene) && defined(BUILD_PlayerControls)
#include "Madgine/controls/playercontroller.h"
#endif
#if defined(BUILD_Scene) && defined(BUILD_Base)
#include "Madgine/scene/scenemanager.h"
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
#include "Madgine/physics/physicsmanager.h"
#include "Madgine/physics/rigidbody.h"
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_Scene)
#include "brick.h"
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
#if defined(BUILD_NodeGraph) && defined(BUILD_Python3Nodes)
#include ""
#endif
#if defined(BUILD_OpenALPlugin) && defined(BUILD_Base)
#include "OpenAL/openalaudiocontext.h"
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
#if defined(BUILD_Tools) && defined(BUILD_ControlsTools)
#include "Madgine_Tools/controls/controlsview.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_OpenGLTools)
#include "OpenGL_Tools/opengltoolconfig.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_UlamTools)
#include "heroeseditor.h"
#include "ulamtool.h"
#endif
#if defined(BUILD_WidgetsTools) && defined(BUILD_Tools)
#include "Madgine_Tools/guieditor/guieditor.h"
#endif
#if defined(BUILD_SceneRendererTools) && defined(BUILD_Tools)
#include "Madgine_Tools/sceneeditor/sceneeditor.h"
#endif
#if defined(BUILD_ClientNodesTools) && defined(BUILD_Tools)
#include ""
#endif
#if defined(BUILD_BulletTools) && defined(BUILD_Tools)
#include "Madgine_Tools/bulletdebug/bulletdebug.h"
#endif
#if defined(BUILD_Python3Tools) && defined(BUILD_Tools)
#include "Madgine_Tools/Python3/python3immediatewindow.h"
#endif
#if defined(BUILD_NodeGraphTools) && defined(BUILD_Tools)
#include "Madgine_Tools/nodegraph/nodegrapheditor.h"
#endif
#if defined(BUILD_AudioLoader) && defined(BUILD_Resources)
#include "audiobufferloader.h"
#include "audiostreamloader.h"
#endif


namespace Engine{
#if defined(BUILD_Resources)
template <>
std::vector<Engine::Resources::ResourceLoaderRegistry::F> Engine::Resources::ResourceLoaderRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_AudioLoader) && defined(BUILD_Resources)
		createComponent<Engine::Audio::AudioBufferLoader>,
#endif
#if defined(BUILD_AudioLoader) && defined(BUILD_Resources)
		createComponent<Engine::Audio::AudioStreamLoader>,
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
		createComponent<Engine::Resources::ImageLoader>,
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
		createComponent<Engine::Render::MeshLoader>,
#endif
#if defined(BUILD_SkeletonLoader) && defined(BUILD_Resources)
		createComponent<Engine::Render::SkeletonLoader>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Resources)
		createComponent<Engine::NodeGraph::NodeGraphLoader>,
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
#if defined(BUILD_Python3) && defined(BUILD_Resources)
		createComponent<Engine::Scripting::Python3::Python3FileLoader>,
#endif
#if defined(BUILD_AnimationLoader) && defined(BUILD_Resources)
		createComponent<Engine::Render::AnimationLoader>,
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
		createComponent<Engine::Render::FontLoader>,
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Resources)
		createComponent<Engine::Physics::CollisionShapeManager>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_Resources)
		createComponent<Engine::Tools::LayoutLoader>,
#endif
#if defined(BUILD_Resources)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_AudioLoader) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_AudioLoader = ACC;
#endif
#if defined(BUILD_AudioLoader) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Audio::AudioBufferLoader>() { return CollectorBaseIndex_ResourceLoaderBase_AudioLoader + 0; }
#endif
#if defined(BUILD_AudioLoader) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Audio::AudioStreamLoader>() { return CollectorBaseIndex_ResourceLoaderBase_AudioLoader + 1; }
#endif
#if defined(BUILD_AudioLoader) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_AudioLoader + 2
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
#if defined(BUILD_NodeGraph) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_NodeGraph = ACC;
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::NodeGraph::NodeGraphLoader>() { return CollectorBaseIndex_ResourceLoaderBase_NodeGraph + 0; }
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
#if defined(BUILD_Python3) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_Python3 = ACC;
#endif
#if defined(BUILD_Python3) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Scripting::Python3::Python3FileLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Python3 + 0; }
#endif
#if defined(BUILD_Python3) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_Python3 + 1
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
#if defined(BUILD_Bullet) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_Bullet = ACC;
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Physics::CollisionShapeManager>() { return CollectorBaseIndex_ResourceLoaderBase_Bullet + 0; }
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_Bullet + 1
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
#if defined(BUILD_Base)
template <>
std::vector<Engine::App::GlobalAPIRegistry::F> Engine::App::GlobalAPIRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Controls) && defined(BUILD_Base)
		createComponent<Engine::Controls::ControlsManager>,
#endif
#if defined(BUILD_Python3) && defined(BUILD_Base)
		createComponent<Engine::Scripting::Python3::Python3Environment>,
#endif
#if defined(BUILD_Scene) && defined(BUILD_Base)
		createComponent<Engine::Serialize::NoParentUnit<Engine::Scene::SceneManager>>,
#endif
#if defined(BUILD_OpenALPlugin) && defined(BUILD_Base)
		createComponent<Engine::Audio::OpenALAudioContext>,
#endif
#if defined(BUILD_Base)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_Controls) && defined(BUILD_Base)
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Controls = ACC;
#endif
#if defined(BUILD_Controls) && defined(BUILD_Base)
template <>
size_t component_index<Engine::Controls::ControlsManager>() { return CollectorBaseIndex_GlobalAPIBase_Controls + 0; }
#endif
#if defined(BUILD_Controls) && defined(BUILD_Base)
#        undef ACC
#        define ACC CollectorBaseIndex_GlobalAPIBase_Controls + 1
#endif
#if defined(BUILD_Python3) && defined(BUILD_Base)
constexpr size_t CollectorBaseIndex_GlobalAPIBase_Python3 = ACC;
#endif
#if defined(BUILD_Python3) && defined(BUILD_Base)
template <>
size_t component_index<Engine::Scripting::Python3::Python3Environment>() { return CollectorBaseIndex_GlobalAPIBase_Python3 + 0; }
#endif
#if defined(BUILD_Python3) && defined(BUILD_Base)
#        undef ACC
#        define ACC CollectorBaseIndex_GlobalAPIBase_Python3 + 1
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
#if defined(BUILD_OpenALPlugin) && defined(BUILD_Base)
constexpr size_t CollectorBaseIndex_GlobalAPIBase_OpenALPlugin = ACC;
#endif
#if defined(BUILD_OpenALPlugin) && defined(BUILD_Base)
template <>
size_t component_index<Engine::Audio::OpenALAudioContext>() { return CollectorBaseIndex_GlobalAPIBase_OpenALPlugin + 0; }
#endif
#if defined(BUILD_OpenALPlugin) && defined(BUILD_Base)
#        undef ACC
#        define ACC CollectorBaseIndex_GlobalAPIBase_OpenALPlugin + 1
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
#if defined(BUILD_Client) && defined(BUILD_ClientControls)
		createComponent<Engine::Controls::ClientControlsManager>,
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
#if defined(BUILD_Client) && defined(BUILD_ClientControls)
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_ClientControls = ACC;
#endif
#if defined(BUILD_Client) && defined(BUILD_ClientControls)
template <>
size_t component_index<Engine::Controls::ClientControlsManager>() { return CollectorBaseIndex_MainWindowComponentBase_ClientControls + 0; }
#endif
#if defined(BUILD_Client) && defined(BUILD_ClientControls)
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_ClientControls + 1
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
#if defined(BUILD_NodeGraph)
template <>
std::vector<Engine::NodeGraph::NodeRegistry::F> Engine::NodeGraph::NodeRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_NodeGraph)
		createComponent<Engine::NodeGraph::FunctionNode>,
#endif
#if defined(BUILD_NodeGraph)
		createComponent<Engine::NodeGraph::PumpNode>,
#endif
#if defined(BUILD_NodeGraph)
		createComponent<Engine::NodeGraph::TestNode>,
#endif
#if defined(BUILD_NodeGraph)
		createComponent<Engine::NodeGraph::VariableNode>,
#endif
#if defined(BUILD_NodeGraph)
		createComponent<Engine::NodeGraph::AdditionNode>,
#endif
#if defined(BUILD_NodeGraph)
		createComponent<Engine::NodeGraph::MultiplicationNode>,
#endif
#if defined(BUILD_NodeGraph)
		createComponent<Engine::NodeGraph::Vector3to4Node>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Python3Nodes)
		createComponent<Engine::Scripting::Python3::Python3FunctionNode>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		createComponent<Engine::Render::GPUBufferNode>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		createComponent<Engine::Render::FrameBufferNode>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		createComponent<Engine::Render::MeshRendererNode>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
		createComponent<Engine::Render::RasterizerNode>,
#endif
#if defined(BUILD_NodeGraph)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_NodeGraph)
constexpr size_t CollectorBaseIndex_NodeBase_NodeGraph = ACC;
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::NodeGraph::FunctionNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 0; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::NodeGraph::PumpNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 1; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::NodeGraph::TestNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 2; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::NodeGraph::VariableNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 3; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::NodeGraph::AdditionNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 4; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::NodeGraph::MultiplicationNode>() { return CollectorBaseIndex_NodeBase_NodeGraph + 5; }
#endif
#if defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::NodeGraph::Vector3to4Node>() { return CollectorBaseIndex_NodeBase_NodeGraph + 6; }
#endif
#if defined(BUILD_NodeGraph)
#        undef ACC
#        define ACC CollectorBaseIndex_NodeBase_NodeGraph + 7
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Python3Nodes)
constexpr size_t CollectorBaseIndex_NodeBase_Python3Nodes = ACC;
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Python3Nodes)
template <>
size_t component_index<Engine::Scripting::Python3::Python3FunctionNode>() { return CollectorBaseIndex_NodeBase_Python3Nodes + 0; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Python3Nodes)
#        undef ACC
#        define ACC CollectorBaseIndex_NodeBase_Python3Nodes + 1
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
constexpr size_t CollectorBaseIndex_NodeBase_ClientNodes = ACC;
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
template <>
size_t component_index<Engine::Render::GPUBufferNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 0; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
template <>
size_t component_index<Engine::Render::FrameBufferNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 1; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
template <>
size_t component_index<Engine::Render::MeshRendererNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 2; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
template <>
size_t component_index<Engine::Render::RasterizerNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 3; }
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_ClientNodes)
#        undef ACC
#        define ACC CollectorBaseIndex_NodeBase_ClientNodes + 4
#endif
#if defined(BUILD_NodeGraph)

#    undef ACC

#endif
#if defined(BUILD_Scene)
template <>
std::vector<Engine::Scene::SceneComponentRegistry::F> Engine::Scene::SceneComponentRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
		createComponent<Engine::Physics::PhysicsManager>,
#endif
#if defined(BUILD_Scene) && defined(BUILD_PlayerControls)
		createComponent<Engine::Controls::PlayerController>,
#endif
#if defined(BUILD_Scene)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
constexpr size_t CollectorBaseIndex_SceneComponentBase_Bullet = ACC;
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
template <>
size_t component_index<Engine::Physics::PhysicsManager>() { return CollectorBaseIndex_SceneComponentBase_Bullet + 0; }
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
#        undef ACC
#        define ACC CollectorBaseIndex_SceneComponentBase_Bullet + 1
#endif
#if defined(BUILD_Scene) && defined(BUILD_PlayerControls)
constexpr size_t CollectorBaseIndex_SceneComponentBase_PlayerControls = ACC;
#endif
#if defined(BUILD_Scene) && defined(BUILD_PlayerControls)
template <>
size_t component_index<Engine::Controls::PlayerController>() { return CollectorBaseIndex_SceneComponentBase_PlayerControls + 0; }
#endif
#if defined(BUILD_Scene) && defined(BUILD_PlayerControls)
#        undef ACC
#        define ACC CollectorBaseIndex_SceneComponentBase_PlayerControls + 1
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
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
		createComponent<Engine::Physics::RigidBody>,
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_Scene)
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
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
constexpr size_t CollectorBaseIndex_EntityComponentBase_Bullet = ACC;
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
template <>
size_t component_index<Engine::Physics::RigidBody>() { return CollectorBaseIndex_EntityComponentBase_Bullet + 0; }
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentBase_Bullet + 1
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_Scene)
constexpr size_t CollectorBaseIndex_EntityComponentBase_ClickBrick = ACC;
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_Scene)
template <>
size_t component_index<ClickBrick::Scene::Brick>() { return CollectorBaseIndex_EntityComponentBase_ClickBrick + 0; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_Scene)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentBase_ClickBrick + 1
#endif
#if defined(BUILD_Scene)

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
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
		createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Physics::RigidBody>>,
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_Scene)
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
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
constexpr size_t CollectorBaseIndex_EntityComponentListBase_Bullet = ACC;
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<Engine::Physics::RigidBody>>() { return CollectorBaseIndex_EntityComponentListBase_Bullet + 0; }
#endif
#if defined(BUILD_Bullet) && defined(BUILD_Scene)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_Bullet + 1
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_Scene)
constexpr size_t CollectorBaseIndex_EntityComponentListBase_ClickBrick = ACC;
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_Scene)
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<ClickBrick::Scene::Brick>>() { return CollectorBaseIndex_EntityComponentListBase_ClickBrick + 0; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_Scene)
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_ClickBrick + 1
#endif
#if defined(BUILD_Scene)

#    undef ACC

#endif
#if defined(BUILD_UI)
template <>
std::vector<Engine::UI::GameHandlerRegistry::F> Engine::UI::GameHandlerRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
		createComponent<ClickBrick::UI::GameManager>,
#endif
#if defined(BUILD_UI)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
constexpr size_t CollectorBaseIndex_GameHandlerBase_ClickBrick = ACC;
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
template <>
size_t component_index<ClickBrick::UI::GameManager>() { return CollectorBaseIndex_GameHandlerBase_ClickBrick + 0; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
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
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
		createComponent<ClickBrick::UI::MainMenuHandler>,
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
		createComponent<ClickBrick::UI::GameHandler>,
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
		createComponent<ClickBrick::UI::GameOverHandler>,
#endif
#if defined(BUILD_UI)

	}; 
}

#    define ACC 0

#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
constexpr size_t CollectorBaseIndex_GuiHandlerBase_ClickBrick = ACC;
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
template <>
size_t component_index<ClickBrick::UI::MainMenuHandler>() { return CollectorBaseIndex_GuiHandlerBase_ClickBrick + 0; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
template <>
size_t component_index<ClickBrick::UI::GameHandler>() { return CollectorBaseIndex_GuiHandlerBase_ClickBrick + 1; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
template <>
size_t component_index<ClickBrick::UI::GameOverHandler>() { return CollectorBaseIndex_GuiHandlerBase_ClickBrick + 2; }
#endif
#if defined(BUILD_ClickBrick) && defined(BUILD_UI)
#        undef ACC
#        define ACC CollectorBaseIndex_GuiHandlerBase_ClickBrick + 3
#endif
#if defined(BUILD_UI)

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
#if defined(BUILD_NodeGraphTools) && defined(BUILD_Tools)
		createComponent<Engine::Tools::NodeGraphEditor>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_ControlsTools)
		createComponent<Engine::Tools::ControlsView>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_UlamTools)
		createComponent<Engine::Tools::UlamTool>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_UlamTools)
		createComponent<Engine::Tools::HeroesEditor>,
#endif
#if defined(BUILD_BulletTools) && defined(BUILD_Tools)
		createComponent<Engine::Tools::BulletDebug>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_OpenGLTools)
		createComponent<Engine::Tools::OpenGLToolConfig>,
#endif
#if defined(BUILD_Python3Tools) && defined(BUILD_Tools)
		createComponent<Engine::Tools::Python3ImmediateWindow>,
#endif
#if defined(BUILD_ClientNodesTools) && defined(BUILD_Tools)
		createComponent<Engine::Client::NodeRendererTester>,
#endif
#if defined(BUILD_SceneRendererTools) && defined(BUILD_Tools)
		createComponent<Engine::Tools::SceneEditor>,
#endif
#if defined(BUILD_WidgetsTools) && defined(BUILD_Tools)
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
#if defined(BUILD_NodeGraphTools) && defined(BUILD_Tools)
constexpr size_t CollectorBaseIndex_ToolBase_NodeGraphTools = ACC;
#endif
#if defined(BUILD_NodeGraphTools) && defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::NodeGraphEditor>() { return CollectorBaseIndex_ToolBase_NodeGraphTools + 0; }
#endif
#if defined(BUILD_NodeGraphTools) && defined(BUILD_Tools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_NodeGraphTools + 1
#endif
#if defined(BUILD_Tools) && defined(BUILD_ControlsTools)
constexpr size_t CollectorBaseIndex_ToolBase_ControlsTools = ACC;
#endif
#if defined(BUILD_Tools) && defined(BUILD_ControlsTools)
template <>
size_t component_index<Engine::Tools::ControlsView>() { return CollectorBaseIndex_ToolBase_ControlsTools + 0; }
#endif
#if defined(BUILD_Tools) && defined(BUILD_ControlsTools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_ControlsTools + 1
#endif
#if defined(BUILD_Tools) && defined(BUILD_UlamTools)
constexpr size_t CollectorBaseIndex_ToolBase_UlamTools = ACC;
#endif
#if defined(BUILD_Tools) && defined(BUILD_UlamTools)
template <>
size_t component_index<Engine::Tools::UlamTool>() { return CollectorBaseIndex_ToolBase_UlamTools + 0; }
#endif
#if defined(BUILD_Tools) && defined(BUILD_UlamTools)
template <>
size_t component_index<Engine::Tools::HeroesEditor>() { return CollectorBaseIndex_ToolBase_UlamTools + 1; }
#endif
#if defined(BUILD_Tools) && defined(BUILD_UlamTools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_UlamTools + 2
#endif
#if defined(BUILD_BulletTools) && defined(BUILD_Tools)
constexpr size_t CollectorBaseIndex_ToolBase_BulletTools = ACC;
#endif
#if defined(BUILD_BulletTools) && defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::BulletDebug>() { return CollectorBaseIndex_ToolBase_BulletTools + 0; }
#endif
#if defined(BUILD_BulletTools) && defined(BUILD_Tools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_BulletTools + 1
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
#if defined(BUILD_Python3Tools) && defined(BUILD_Tools)
constexpr size_t CollectorBaseIndex_ToolBase_Python3Tools = ACC;
#endif
#if defined(BUILD_Python3Tools) && defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::Python3ImmediateWindow>() { return CollectorBaseIndex_ToolBase_Python3Tools + 0; }
#endif
#if defined(BUILD_Python3Tools) && defined(BUILD_Tools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_Python3Tools + 1
#endif
#if defined(BUILD_ClientNodesTools) && defined(BUILD_Tools)
constexpr size_t CollectorBaseIndex_ToolBase_ClientNodesTools = ACC;
#endif
#if defined(BUILD_ClientNodesTools) && defined(BUILD_Tools)
template <>
size_t component_index<Engine::Client::NodeRendererTester>() { return CollectorBaseIndex_ToolBase_ClientNodesTools + 0; }
#endif
#if defined(BUILD_ClientNodesTools) && defined(BUILD_Tools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_ClientNodesTools + 1
#endif
#if defined(BUILD_SceneRendererTools) && defined(BUILD_Tools)
constexpr size_t CollectorBaseIndex_ToolBase_SceneRendererTools = ACC;
#endif
#if defined(BUILD_SceneRendererTools) && defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::SceneEditor>() { return CollectorBaseIndex_ToolBase_SceneRendererTools + 0; }
#endif
#if defined(BUILD_SceneRendererTools) && defined(BUILD_Tools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_SceneRendererTools + 1
#endif
#if defined(BUILD_WidgetsTools) && defined(BUILD_Tools)
constexpr size_t CollectorBaseIndex_ToolBase_WidgetsTools = ACC;
#endif
#if defined(BUILD_WidgetsTools) && defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::GuiEditor>() { return CollectorBaseIndex_ToolBase_WidgetsTools + 0; }
#endif
#if defined(BUILD_WidgetsTools) && defined(BUILD_Tools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_WidgetsTools + 1
#endif
#if defined(BUILD_Tools)

#    undef ACC

#endif
}
