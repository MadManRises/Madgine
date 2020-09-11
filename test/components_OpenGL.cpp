#include "Modules/moduleslib.h"
#include "Modules/uniquecomponent/uniquecomponentregistry.h"
#ifdef BUILD_OpenGL
#    include "OpenGL/opengllib.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scenelib.h"
#endif
#ifdef BUILD_Widgets
#    include "Madgine/widgetslib.h"
#endif
#ifdef BUILD_FontLoader
#    include "fontloaderlib.h"
#endif
#ifdef BUILD_UI
#    include "Madgine/uilib.h"
#endif
#ifdef BUILD_Modules
#    include "Modules/moduleslib.h"
#endif
#ifdef BUILD_AnimationLoader
#    include "animationloaderlib.h"
#endif
#ifdef BUILD_MeshLoader
#    include "meshloaderlib.h"
#endif
#ifdef BUILD_SkeletonLoader
#    include "skeletonloaderlib.h"
#endif
#ifdef BUILD_ImageLoader
#    include "imageloaderlib.h"
#endif
#ifdef BUILD_ClickBrick
#    include "clickbricklib.h"
#endif
#ifdef BUILD_Client
#    include "Madgine/clientlib.h"
#endif
#ifdef BUILD_Base
#    include "Madgine/baselib.h"
#endif
#ifdef BUILD_Base
#    include "Madgine/app/globalapicollector.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/scenemanager.h"
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
#ifdef BUILD_MeshLoader
#    include "meshloader.h"
#endif
#ifdef BUILD_SkeletonLoader
#    include "skeletonloader.h"
#endif
#ifdef BUILD_AnimationLoader
#    include "animationloader.h"
#endif
#ifdef BUILD_ImageLoader
#    include "imageloader.h"
#endif
#ifdef BUILD_FontLoader
#    include "fontloader.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/entitycomponentcollector.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/components/animation.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/components/mesh.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/components/skeleton.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/components/transform.h"
#endif
#ifdef BUILD_ClickBrick
#    include "brick.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/entitycomponentcollector.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/components/animation.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/components/mesh.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/components/skeleton.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/entity/components/transform.h"
#endif
#ifdef BUILD_ClickBrick
#    include "brick.h"
#endif
#ifdef BUILD_Scene
#    include "Madgine/scene/scenecomponentcollector.h"
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
#ifdef BUILD_Client
#    include "Madgine/window/mainwindowcomponentcollector.h"
#endif
#ifdef BUILD_Widgets
#    include "Madgine/widgets/widgetmanager.h"
#endif
#ifdef BUILD_UI
#    include "Madgine/ui/uimanager.h"
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
std::vector<Engine::App::GlobalAPIRegistry::F> Engine::App::GlobalAPIRegistry::sComponents()
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
std::vector<Engine::Render::RenderContextRegistry::F> Engine::Render::RenderContextRegistry::sComponents()
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
std::vector<Engine::Resources::ResourceLoaderRegistry::F> Engine::Resources::ResourceLoaderRegistry::sComponents()
{
	return {
#    ifdef BUILD_OpenGL
		createComponent<Engine::Render::OpenGLMeshLoader>,
		createComponent<Engine::Render::OpenGLProgramLoader>,
		createComponent<Engine::Render::OpenGLShaderLoader>,
		createComponent<Engine::Render::OpenGLTextureLoader>,
#    endif
#    ifdef BUILD_MeshLoader
		createComponent<Engine::Render::MeshLoader>,
#    endif
#    ifdef BUILD_SkeletonLoader
		createComponent<Engine::Render::SkeletonLoader>,
#    endif
#    ifdef BUILD_AnimationLoader
		createComponent<Engine::Render::AnimationLoader>,
#    endif
#    ifdef BUILD_ImageLoader
		createComponent<Engine::Resources::ImageLoader>,
#    endif
#    ifdef BUILD_FontLoader
		createComponent<Engine::Render::FontLoader>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_OpenGL
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_OpenGL = ACC;
template <>
size_t component_index<Engine::Render::OpenGLMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 0; }
template <>
size_t component_index<Engine::Render::GPUMeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 0; }
template <>
size_t component_index<Engine::Render::OpenGLProgramLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
template <>
size_t component_index<Engine::Render::ProgramLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 1; }
template <>
size_t component_index<Engine::Render::OpenGLShaderLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 2; }
template <>
size_t component_index<Engine::Render::OpenGLTextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 3; }
template <>
size_t component_index<Engine::Render::TextureLoader>() { return CollectorBaseIndex_ResourceLoaderBase_OpenGL + 3; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_OpenGL + 4
#    endif
#    ifdef BUILD_MeshLoader
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_MeshLoader = ACC;
template <>
size_t component_index<Engine::Render::MeshLoader>() { return CollectorBaseIndex_ResourceLoaderBase_MeshLoader + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_MeshLoader + 1
#    endif
#    ifdef BUILD_SkeletonLoader
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader = ACC;
template <>
size_t component_index<Engine::Render::SkeletonLoader>() { return CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_SkeletonLoader + 1
#    endif
#    ifdef BUILD_AnimationLoader
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_AnimationLoader = ACC;
template <>
size_t component_index<Engine::Render::AnimationLoader>() { return CollectorBaseIndex_ResourceLoaderBase_AnimationLoader + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_AnimationLoader + 1
#    endif
#    ifdef BUILD_ImageLoader
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_ImageLoader = ACC;
template <>
size_t component_index<Engine::Resources::ImageLoader>() { return CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_ImageLoader + 1
#    endif
#    ifdef BUILD_FontLoader
constexpr size_t CollectorBaseIndex_ResourceLoaderBase_FontLoader = ACC;
template <>
size_t component_index<Engine::Render::FontLoader>() { return CollectorBaseIndex_ResourceLoaderBase_FontLoader + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_ResourceLoaderBase_FontLoader + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Scene
template <>
std::vector<Engine::Scene::Entity::EntityComponentListRegistry::F> Engine::Scene::Entity::EntityComponentListRegistry::sComponents()
{
	return {
#    ifdef BUILD_Scene
		createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>>,
		createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Mesh>>,
		createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Skeleton>>,
		createComponent<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Transform>>,
#    endif
#    ifdef BUILD_ClickBrick
		createComponent<Engine::Scene::Entity::EntityComponentList<ClickBrick::Scene::Brick>>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_Scene
constexpr size_t CollectorBaseIndex_EntityComponentListBase_Scene = ACC;
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Animation>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 0; }
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Mesh>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 1; }
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Skeleton>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 2; }
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Transform>>() { return CollectorBaseIndex_EntityComponentListBase_Scene + 3; }
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_Scene + 4
#    endif
#    ifdef BUILD_ClickBrick
constexpr size_t CollectorBaseIndex_EntityComponentListBase_ClickBrick = ACC;
template <>
size_t component_index<Engine::Scene::Entity::EntityComponentList<ClickBrick::Scene::Brick>>() { return CollectorBaseIndex_EntityComponentListBase_ClickBrick + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentListBase_ClickBrick + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Scene
template <>
std::vector<Engine::Scene::Entity::EntityComponentRegistry::F> Engine::Scene::Entity::EntityComponentRegistry::sComponents()
{
	return {
#    ifdef BUILD_Scene
		createComponent<Engine::Scene::Entity::Animation>,
		createComponent<Engine::Scene::Entity::Mesh>,
		createComponent<Engine::Scene::Entity::Skeleton>,
		createComponent<Engine::Scene::Entity::Transform>,
#    endif
#    ifdef BUILD_ClickBrick
		createComponent<ClickBrick::Scene::Brick>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_Scene
constexpr size_t CollectorBaseIndex_EntityComponentBase_Scene = ACC;
template <>
size_t component_index<Engine::Scene::Entity::Animation>() { return CollectorBaseIndex_EntityComponentBase_Scene + 0; }
template <>
size_t component_index<Engine::Scene::Entity::Mesh>() { return CollectorBaseIndex_EntityComponentBase_Scene + 1; }
template <>
size_t component_index<Engine::Scene::Entity::Skeleton>() { return CollectorBaseIndex_EntityComponentBase_Scene + 2; }
template <>
size_t component_index<Engine::Scene::Entity::Transform>() { return CollectorBaseIndex_EntityComponentBase_Scene + 3; }
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentBase_Scene + 4
#    endif
#    ifdef BUILD_ClickBrick
constexpr size_t CollectorBaseIndex_EntityComponentBase_ClickBrick = ACC;
template <>
size_t component_index<ClickBrick::Scene::Brick>() { return CollectorBaseIndex_EntityComponentBase_ClickBrick + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_EntityComponentBase_ClickBrick + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_Scene
template <>
std::vector<Engine::Scene::SceneComponentRegistry::F> Engine::Scene::SceneComponentRegistry::sComponents()
{
	return {

	}; 
}

#    define ACC 0


#    undef ACC

#endif
#ifdef BUILD_UI
template <>
std::vector<Engine::UI::GameHandlerRegistry::F> Engine::UI::GameHandlerRegistry::sComponents()
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
std::vector<Engine::UI::GuiHandlerRegistry::F> Engine::UI::GuiHandlerRegistry::sComponents()
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
#ifdef BUILD_Client
template <>
std::vector<Engine::Window::MainWindowComponentRegistry::F> Engine::Window::MainWindowComponentRegistry::sComponents()
{
	return {
#    ifdef BUILD_Widgets
		createComponent<Engine::Widgets::WidgetManager>,
#    endif
#    ifdef BUILD_UI
		createComponent<Engine::UI::UIManager>,
#    endif

	}; 
}

#    define ACC 0

#    ifdef BUILD_Widgets
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_Widgets = ACC;
template <>
size_t component_index<Engine::Widgets::WidgetManager>() { return CollectorBaseIndex_MainWindowComponentBase_Widgets + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_Widgets + 1
#    endif
#    ifdef BUILD_UI
constexpr size_t CollectorBaseIndex_MainWindowComponentBase_UI = ACC;
template <>
size_t component_index<Engine::UI::UIManager>() { return CollectorBaseIndex_MainWindowComponentBase_UI + 0; }
#        undef ACC
#        define ACC CollectorBaseIndex_MainWindowComponentBase_UI + 1
#    endif

#    undef ACC

#endif
#ifdef BUILD_TestShared
template <>
std::vector<Test::TestRegistry::F> Test::TestRegistry::sComponents()
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
