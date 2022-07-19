#include "Modules/moduleslib.h"
#include "Modules/uniquecomponent/uniquecomponentregistry.h"
#if defined(BUILD_NodeGraph)
#include "Madgine/nodegraphlib.h"
#endif
#if defined(BUILD_ClientTools)
#include "Madgine_Tools/clienttoolslib.h"
#endif
#if defined(BUILD_Tools)
#include "toolslib.h"
#endif
#if defined(BUILD_OpenGL)
#include "OpenGL/opengllib.h"
#endif
#if defined(BUILD_ClientNodesTools)
#include "Madgine/clientnodestoolslib.h"
#endif
#if defined(BUILD_NodeGraphTools)
#include "Madgine_Tools/nodegraphtoolslib.h"
#endif
#if defined(BUILD_OpenGLTools)
#include "OpenGL_Tools/opengltoolslib.h"
#endif
#if defined(BUILD_WidgetsTools)
#include "Madgine_Tools/widgetstoolslib.h"
#endif
#if defined(BUILD_UI)
#include "Madgine/uilib.h"
#endif
#if defined(BUILD_Base)
#include "Madgine/baselib.h"
#endif
#if defined(BUILD_Widgets)
#include "Madgine/widgetslib.h"
#endif
#if defined(BUILD_Client)
#include "Madgine/clientlib.h"
#endif
#if defined(BUILD_MeshLoader)
#include "meshloaderlib.h"
#endif
#if defined(BUILD_FontLoader)
#include "fontloaderlib.h"
#endif
#if defined(BUILD_ImageLoader)
#include "imageloaderlib.h"
#endif
#if defined(BUILD_ClientNodes)
#include "Madgine/clientnodeslib.h"
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
#if defined(BUILD_Tools)
#include "filesystem/filebrowser.h"
#include "inject/injectortool.h"
#include "inspector/functiontool.h"
#include "inspector/inspector.h"
#include "logviewer/logviewer.h"
#include "metrics/metrics.h"
#include "profiler/profiler.h"
#include "renderer/imguidemo.h"
#include "tasktracker/tasktracker.h"
#include "testtool/testtool.h"
#include "toolscollector.h"
#endif
#if defined(BUILD_ClientTools) && defined(BUILD_Tools)
#include "Madgine_Tools/gamepad/gamepadviewer.h"
#include "Madgine_Tools/imgui/projectmanager.h"
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
#include "OpenGL/openglmeshloader.h"
#include "OpenGL/openglpipelineloader.h"
#include "OpenGL/openglshaderloader.h"
#include "OpenGL/opengltextureloader.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_ClientNodesTools)
#include "Madgine/client/nodes/noderenderertester.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_NodeGraphTools)
#include "Madgine_Tools/nodegraph/nodegrapheditor.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_OpenGLTools)
#include "OpenGL_Tools/opengltoolconfig.h"
#endif
#if defined(BUILD_Tools) && defined(BUILD_WidgetsTools)
#include "Madgine_Tools/guieditor/guieditor.h"
#endif
#if defined(BUILD_UI)
#include "Madgine/input/handlercollector.h"
#endif
#if defined(BUILD_Base)
#include "Madgine/base/globalapicollector.h"
#endif
#if defined(BUILD_Client)
#include "Madgine/render/rendercontextcollector.h"
#include "Madgine/window/mainwindowcomponentcollector.h"
#endif
#if defined(BUILD_Client) && defined(BUILD_Resources)
#include "Madgine/render/shadinglanguage/slloader.h"
#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGL)
#include "OpenGL/openglrendercontext.h"
#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGLTools)
#include "OpenGL_Tools/imgui/openglimroot.h"
#endif
#if defined(BUILD_Widgets) && defined(BUILD_Client)
#include "Madgine/widgets/widgetmanager.h"
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
#include "meshloader.h"
#endif
#if defined(BUILD_FontLoader) && defined(BUILD_Resources)
#include "fontloader.h"
#endif
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
#include "imageloader.h"
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
#include "Madgine/client/nodes/framebuffernode.h"
#include "Madgine/client/nodes/gpubuffernode.h"
#include "Madgine/client/nodes/meshrenderernode.h"
#include "Madgine/client/nodes/rasterizernode.h"
#endif


namespace Engine{
namespace UniqueComponent{
#if defined(BUILD_Resources)
template <>
std::vector<Engine::Resources::ResourceLoaderRegistry::F> Engine::Resources::ResourceLoaderRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Client) && defined(BUILD_Resources)
		createComponent<Engine::Render::SlLoader>,
#endif
#if defined(BUILD_NodeGraph) && defined(BUILD_Resources)
		createComponent<Engine::NodeGraph::NodeGraphLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		createComponent<Engine::Render::OpenGLMeshLoader>,
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
		createComponent<Engine::Render::OpenGLPipelineLoader>,
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
#if defined(BUILD_ImageLoader) && defined(BUILD_Resources)
		createComponent<Engine::Resources::ImageLoader>,
#endif
#if defined(BUILD_MeshLoader) && defined(BUILD_Resources)
		createComponent<Engine::Render::MeshLoader>,
#endif
#if defined(BUILD_Resources)

	}; 
}
#endif
#if defined(BUILD_Resources)

#    define ACC 0

#endif
#if defined(BUILD_Client) && defined(BUILD_Resources)
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_Client = ACC;
#endif
#if defined(BUILD_Client) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::SlLoader>() { return CollectorBaseIndex_ResourceLoaderBase_Client + 0; }
#endif
#if defined(BUILD_Client) && defined(BUILD_Resources)
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_Client + 1
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
size_t component_index<Engine::Render::OpenGLPipelineLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
#endif
#if defined(BUILD_OpenGL) && defined(BUILD_Resources)
template <>
size_t component_index<Engine::Render::PipelineLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
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
#if defined(BUILD_Resources)

#    undef ACC

#endif
#if defined(BUILD_Client)
template <>
std::vector<Engine::Render::RenderContextRegistry::F> Engine::Render::RenderContextRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGL)
		createComponent<Engine::Render::OpenGLRenderContext>,
#endif
#if defined(BUILD_Client)

	}; 
}
#endif
#if defined(BUILD_Client)

#    define ACC 0

#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGL)
constexpr size_t CollectorBaseIndex_RenderContext_OpenGL = ACC;
#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGL)
template <>
size_t component_index<Engine::Render::OpenGLRenderContext>() { return CollectorBaseIndex_RenderContext_OpenGL + 0; }
#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGL)
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
#if defined(BUILD_Widgets) && defined(BUILD_Client)
		createComponent<Engine::Widgets::WidgetManager>,
#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGLTools)
		createComponent<Engine::Tools::OpenGLImRoot>,
#endif
#if defined(BUILD_Client)

	}; 
}
#endif
#if defined(BUILD_Client)

#    define ACC 0

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
#if defined(BUILD_Client) && defined(BUILD_OpenGLTools)
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_OpenGLTools = ACC;
#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGLTools)
template <>
size_t component_index<Engine::Tools::OpenGLImRoot>() { return CollectorBaseIndex_MainWindowComponentBase_OpenGLTools + 0; }
#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGLTools)
template <>
size_t component_index<Engine::Tools::ClientImRoot>() { return CollectorBaseIndex_MainWindowComponentBase_OpenGLTools + 0; }
#endif
#if defined(BUILD_Client) && defined(BUILD_OpenGLTools)
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_OpenGLTools + 1
#endif
#if defined(BUILD_Client)

#    undef ACC

#endif
#if defined(BUILD_Base)
template <>
std::vector<Engine::Base::GlobalAPIRegistry::F> Engine::Base::GlobalAPIRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Base)

	}; 
}
#endif
#if defined(BUILD_Base)

#    define ACC 0

#endif
#if defined(BUILD_Base)

#    undef ACC

#endif
#if defined(BUILD_UI)
template <>
std::vector<Engine::Input::GameHandlerRegistry::F> Engine::Input::GameHandlerRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_UI)

	}; 
}
#endif
#if defined(BUILD_UI)

#    define ACC 0

#endif
#if defined(BUILD_UI)

#    undef ACC

#endif
#if defined(BUILD_UI)
template <>
std::vector<Engine::Input::GuiHandlerRegistry::F> Engine::Input::GuiHandlerRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_UI)

	}; 
}
#endif
#if defined(BUILD_UI)

#    define ACC 0

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
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
		createComponent<Engine::Render::GPUBufferNode>,
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
		createComponent<Engine::Render::FrameBufferNode>,
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
		createComponent<Engine::Render::MeshRendererNode>,
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
		createComponent<Engine::Render::RasterizerNode>,
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
#if defined(BUILD_NodeGraph)

	}; 
}
#endif
#if defined(BUILD_NodeGraph)

#    define ACC 0

#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
constexpr size_t CollectorBaseIndex_NodeBase_ClientNodes = ACC;
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::Render::GPUBufferNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 0; }
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::Render::FrameBufferNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 1; }
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::Render::MeshRendererNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 2; }
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
template <>
size_t component_index<Engine::Render::RasterizerNode>() { return CollectorBaseIndex_NodeBase_ClientNodes + 3; }
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
#        undef ACC
#        define ACC CollectorBaseIndex_NodeBase_ClientNodes + 4
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
#if defined(BUILD_NodeGraph)

#    undef ACC

#endif
#if defined(BUILD_NodeGraph)
template <>
std::map<std::string_view, size_t> Engine::NodeGraph::NodeRegistry::sComponentsByName()
{
	return {
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
		{"GPUBufferNode", CollectorBaseIndex_NodeBase_ClientNodes + 0},
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
		{"FrameBufferNode", CollectorBaseIndex_NodeBase_ClientNodes + 1},
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
		{"MeshRendererNode", CollectorBaseIndex_NodeBase_ClientNodes + 2},
#endif
#if defined(BUILD_ClientNodes) && defined(BUILD_NodeGraph)
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
#if defined(BUILD_Tools)
template <>
std::vector<Engine::Tools::ToolsRegistry::F> Engine::Tools::ToolsRegistry::sComponents()
{
	return {
#endif
#if defined(BUILD_Tools) && defined(BUILD_ClientNodesTools)
		createComponent<Engine::Tools::NodeRendererTester>,
#endif
#if defined(BUILD_ClientTools) && defined(BUILD_Tools)
		createComponent<Engine::Tools::GamepadViewer>,
#endif
#if defined(BUILD_ClientTools) && defined(BUILD_Tools)
		createComponent<Engine::Tools::ProjectManager>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_NodeGraphTools)
		createComponent<Engine::Tools::NodeGraphEditor>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_OpenGLTools)
		createComponent<Engine::Tools::OpenGLToolConfig>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::FileBrowser>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::InjectorTool>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::FunctionTool>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::Inspector>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::LogViewer>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::Metrics>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::Profiler>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::ImGuiDemo>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::TaskTracker>,
#endif
#if defined(BUILD_Tools)
		createComponent<Engine::Tools::TestTool>,
#endif
#if defined(BUILD_Tools) && defined(BUILD_WidgetsTools)
		createComponent<Engine::Tools::GuiEditor>,
#endif
#if defined(BUILD_Tools)

	}; 
}
#endif
#if defined(BUILD_Tools)

#    define ACC 0

#endif
#if defined(BUILD_Tools) && defined(BUILD_ClientNodesTools)
constexpr size_t CollectorBaseIndex_ToolBase_ClientNodesTools = ACC;
#endif
#if defined(BUILD_Tools) && defined(BUILD_ClientNodesTools)
template <>
size_t component_index<Engine::Tools::NodeRendererTester>() { return CollectorBaseIndex_ToolBase_ClientNodesTools + 0; }
#endif
#if defined(BUILD_Tools) && defined(BUILD_ClientNodesTools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_ClientNodesTools + 1
#endif
#if defined(BUILD_ClientTools) && defined(BUILD_Tools)
constexpr size_t CollectorBaseIndex_ToolBase_ClientTools = ACC;
#endif
#if defined(BUILD_ClientTools) && defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::GamepadViewer>() { return CollectorBaseIndex_ToolBase_ClientTools + 0; }
#endif
#if defined(BUILD_ClientTools) && defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::ProjectManager>() { return CollectorBaseIndex_ToolBase_ClientTools + 1; }
#endif
#if defined(BUILD_ClientTools) && defined(BUILD_Tools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_ClientTools + 2
#endif
#if defined(BUILD_Tools) && defined(BUILD_NodeGraphTools)
constexpr size_t CollectorBaseIndex_ToolBase_NodeGraphTools = ACC;
#endif
#if defined(BUILD_Tools) && defined(BUILD_NodeGraphTools)
template <>
size_t component_index<Engine::Tools::NodeGraphEditor>() { return CollectorBaseIndex_ToolBase_NodeGraphTools + 0; }
#endif
#if defined(BUILD_Tools) && defined(BUILD_NodeGraphTools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_NodeGraphTools + 1
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
#if defined(BUILD_Tools)
constexpr size_t CollectorBaseIndex_ToolBase_Tools = ACC;
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::FileBrowser>() { return CollectorBaseIndex_ToolBase_Tools + 0; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::InjectorTool>() { return CollectorBaseIndex_ToolBase_Tools + 1; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::FunctionTool>() { return CollectorBaseIndex_ToolBase_Tools + 2; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::Inspector>() { return CollectorBaseIndex_ToolBase_Tools + 3; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::LogViewer>() { return CollectorBaseIndex_ToolBase_Tools + 4; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::Metrics>() { return CollectorBaseIndex_ToolBase_Tools + 5; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::Profiler>() { return CollectorBaseIndex_ToolBase_Tools + 6; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::ImGuiDemo>() { return CollectorBaseIndex_ToolBase_Tools + 7; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::TaskTracker>() { return CollectorBaseIndex_ToolBase_Tools + 8; }
#endif
#if defined(BUILD_Tools)
template <>
size_t component_index<Engine::Tools::TestTool>() { return CollectorBaseIndex_ToolBase_Tools + 9; }
#endif
#if defined(BUILD_Tools)
#        undef ACC
#        define ACC CollectorBaseIndex_ToolBase_Tools + 10
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
#endif
#if defined(BUILD_TestShared)

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
}
