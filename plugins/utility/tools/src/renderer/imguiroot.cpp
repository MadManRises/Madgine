#include "../toolslib.h"

#include "imguiroot.h"

#ifdef BUILD_PLUGIN_Ogre
#include "ogre/ogreimguimanager.h"
#endif
#ifdef BUILD_PLUGIN_OpenGL
#include "opengl/openglimguimanager.h"
#endif

#include "Madgine/app/application.h"

#include "Modules/math/vector3.h"
#include "gui/guisystem.h"
#include "gui/widgets/toplevelwindow.h"

#include "input/inputhandler.h"

#include "Modules/debug/profiler/profiler.h"

#include "../toolbase.h"

#include "Modules/plugins/pluginmanager.h"

#include "Modules/signalslot/taskqueue.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "imguimanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "Modules/keyvalue/scopeiterator.h"

THREADLOCAL(ImGuiContext *)
GImGui;

UNIQUECOMPONENT(Engine::Tools::ImGuiRoot);

namespace Engine {

namespace Tools {

    std::unique_ptr<ImGuiManager> createOpenGlManager(GUI::TopLevelWindow &);

    void *ToolReadOpen(ImGuiContext *ctx, ImGuiSettingsHandler *handler, const char *name) // Read: Called when entering into a new ini entry e.g. "[Window][Name]"
    {
        return nullptr;
    }

    void ToolReadLine(ImGuiContext *ctx, ImGuiSettingsHandler *handler, void *entry, const char *line) // Read: Called for every line of text within an ini entry
    {
    }

    void ToolWriteAll(ImGuiContext *ctx, ImGuiSettingsHandler *handler, ImGuiTextBuffer *out_buf) // Write: Output every entries into 'out_buf'
    {
        ImGuiRoot *root = static_cast<ImGuiRoot *>(handler->UserData);
        int i = 0;
        for (auto it = root->tools().typedBegin(); it != root->tools().typedEnd(); ++it) {
            out_buf->appendf("[Tool][%s]\n", (*std::next(root->tools().begin(), i++))->key());
            for (std::pair<const char *, ValueType> p : (*it)) {
                if (p.second.type() != Engine::ValueType::Type::BoundApiMethodValue)
					out_buf->appendf("%s=%s\n", p.first, p.second.toString().c_str());
            }
            out_buf->append("\n");
        }
    }

    ImGuiRoot::ImGuiRoot(GUI::TopLevelWindow &window)
        : UniqueComponent(window)
        , mCollector(*this)
    {
    }

    ImGuiRoot::~ImGuiRoot()
    {
    }

    bool ImGuiRoot::init()
    {
#if ENABLE_PLUGINS
        Plugins::PluginManager::getSingleton()["Renderer"].addListener(this);
#else
        createManager();
#endif

        ImGuiSettingsHandler ini_handler;
        ini_handler.TypeName = "Tool";
        ini_handler.TypeHash = ImHashStr("Tool");
        ini_handler.ReadOpenFn = ToolReadOpen;
        ini_handler.ReadLineFn = ToolReadLine;
        ini_handler.WriteAllFn = ToolWriteAll;
        ini_handler.UserData = this;
        GImGui->SettingsHandlers.push_back(ini_handler);

        for (const std::unique_ptr<ToolBase> &tool : mCollector) {
            tool->callInit();
        }

        return true;
    }

    void ImGuiRoot::finalize()
    {
        for (const std::unique_ptr<ToolBase> &tool : mCollector) {
            tool->callFinalize();
        }

#if ENABLE_PLUGINS
        Plugins::PluginManager::getSingleton()["Renderer"].removeListener(this);
#else
        destroyManager();
#endif
    }

    bool ImGuiRoot::frameStarted(std::chrono::microseconds timeSinceLastFrame)
    {
        return true;
    }

    bool ImGuiRoot::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
    {
        PROFILE_NAMED("ImGui - Rendering");

        mManager->newFrame((float)timeSinceLastFrame.count() / 1000000.0f);

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        bool open = true;
        ImGui::Begin("Madgine Root Window", &open, window_flags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiID dockspace_id = ImGui::GetID("MadgineDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        ImGuiDockNode *node = ImGui::DockBuilderGetNode(dockspace_id);

        mManager->setCentralNode(node->CentralNode);

        if (ImGui::BeginMainMenuBar()) {

            mManager->setMenuHeight(ImGui::GetWindowSize().y);

            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Quit")) {
                    app().shutdown();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools")) {
                for (const std::unique_ptr<ToolBase> &tool : mCollector) {
                    bool visible = tool->isVisible();
                    ImGui::MenuItem(tool->key(), "", &visible);
                    tool->setVisible(visible);
                }
                ImGui::EndMenu();
            }
            for (const std::unique_ptr<ToolBase> &tool : mCollector) {
                tool->renderMenu();
            }
            ImGui::EndMainMenuBar();
        }

        for (const std::unique_ptr<ToolBase> &tool : mCollector) {
            tool->update();
        }

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGui::End();
        }

        return true;
    }

#if ENABLE_PLUGINS
    bool ImGuiRoot::aboutToUnloadPlugin(const Plugins::Plugin *p)
    {
        //app().frameLoop().queue([this]() {
        destroyManager();
        //});
        return true;
    }

    void ImGuiRoot::onPluginLoad(const Plugins::Plugin *p)
    {
        createManager();
    }

#endif

    void ImGuiRoot::createManager()
    {
        assert(!mManager);
        IF_PLUGIN(OpenGL)
        mManager = createOpenGlManager(mWindow);
        else THROW_PLUGIN("No ImGui-Manager available!");
        mManager->init();
        //mManager->newFrame(0.0f);
        app(false).addFrameListener(this);
    }

    void ImGuiRoot::destroyManager()
    {
        assert(mManager);
        app(false).removeFrameListener(this);
        ImGui::EndFrame();
        mManager->finalize();
        mManager.reset();
    }

    const ToolsContainer<std::vector> &ImGuiRoot::tools()
    {
        return mCollector;
    }

    ToolBase &ImGuiRoot::getToolComponent(size_t index, bool init)
    {
        ToolBase &tool = mCollector.get(index);
        if (init) {
            checkInitState();
            tool.callInit();
        }
        return tool.getSelf(init);
    }

}
}

METATABLE_BEGIN(Engine::Tools::ImGuiRoot)
READONLY_PROPERTY(Tools, tools)
METATABLE_END(Engine::Tools::ImGuiRoot)

RegisterType(Engine::Tools::ImGuiRoot);