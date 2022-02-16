#include "../toolslib.h"

#include "imroot.h"

#include "Modules/debug/profiler/profile.h"

#include "../toolbase.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "im3d/im3d.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/serialize/formatter/iniformatter.h"

#include "Generic/container/safeiterator.h"

#include "Meta/serialize/streams/operations.h"

#include "Meta/serialize/streams/serializestreamdata.h"

#include "Generic/container/transformIt.h"

#include "Meta/serialize/statetransmissionflags.h"

/*
METATABLE_BEGIN(Engine::Tools::ImRoot)
READONLY_PROPERTY(Tools, tools)
METATABLE_END(Engine::Tools::ImRoot)
*/

namespace Engine {

namespace Tools {

    void *ToolReadOpen(ImGuiContext *ctx, ImGuiSettingsHandler *handler, const char *name) // Read: Called when entering into a new ini entry e.g. "[Window][Name]"
    {
        ImRoot *root = static_cast<ImRoot *>(handler->UserData);

        root->finishToolRead();

        root->mToolReadBuffer.str("");
        for (ToolBase *tool : uniquePtrToPtr(root->tools())) {
            if (tool->key() == name) {
                root->mToolReadTool = tool;
                return tool;
            }
        }
        return nullptr;
    }

    void ToolReadLine(ImGuiContext *ctx, ImGuiSettingsHandler *handler, void *entry, const char *line) // Read: Called for every line of text within an ini entry
    {
        if (strlen(line) > 0) {
            static_cast<ImRoot *>(handler->UserData)->mToolReadBuffer << line << "\n";
        }
    }

    void ToolWriteAll(ImGuiContext *ctx, ImGuiSettingsHandler *handler, ImGuiTextBuffer *out_buf) // Write: Output every entry into 'out_buf'
    {
        auto buf = std::make_unique<std::stringbuf>();
        std::stringbuf *outBuffer = buf.get();
        Serialize::FormattedSerializeStream out { std::make_unique<Serialize::IniFormatter>(), { std::move(buf) } };

        ImRoot *root = static_cast<ImRoot *>(handler->UserData);
        for (ToolBase *tool : uniquePtrToPtr(root->tools())) {
            std::string name = std::string { tool->key() };
            out_buf->appendf("[Tool][%s]\n", name.c_str());

            Serialize::SerializableUnitPtr { tool }.writeState(out, nullptr, {}, Serialize::StateTransmissionFlags_SkipId);
            out_buf->append(outBuffer->str().c_str());
            outBuffer->str("");

            out_buf->append("\n");
        }
    }

    ImRoot::ImRoot()
        : mCollector(*this)
    {
    }

    ImRoot::~ImRoot()
    {
    }

    Threading::Task<bool> ImRoot::init()
    {

        ImGuiSettingsHandler ini_handler;
        ini_handler.TypeName = "Tool";
        ini_handler.TypeHash = ImHashStr("Tool");
        ini_handler.ReadOpenFn = ToolReadOpen;
        ini_handler.ReadLineFn = ToolReadLine;
        ini_handler.WriteAllFn = ToolWriteAll;
        ini_handler.UserData = this;
        GImGui->SettingsHandlers.push_back(ini_handler);

        for (const std::unique_ptr<ToolBase> &tool : mCollector) {
            bool result = co_await tool->callInit();
            assert(result);
        }

        co_return true;
    }

    Threading::Task<void> ImRoot::finalize()
    {

        for (const std::unique_ptr<ToolBase> &tool : mCollector) {
            co_await tool->callFinalize();
        }
    }

    void ImRoot::render()
    {
        PROFILE_NAMED("ImGui - Rendering");

        ImGuiIO &io = ImGui::GetIO();

        ImGuiViewport *viewport = ImGui::GetMainViewport();

        ImGui::NewFrame();
        Im3D::NewFrame();

        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {

            ImGuiID dockspace_id = ImGui::GetID("MadgineDockSpace");

            ImGuiDockNodeFlags dockspace_flags = /*ImGuiDockNodeFlags_NoDockingInCentralNode | */ ImGuiDockNodeFlags_PassthruCentralNode;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

            ImGui::SetNextWindowPos(viewport->Pos);
            ImVec2 size = viewport->Size;
            size.y -= ImGui::GetFrameHeight();// remove status bar height
            ImGui::SetNextWindowSize(size); 
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
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            mDockNode = ImGui::DockBuilderGetNode(dockspace_id)->CentralNode;
        }

        if (ImGui::BeginMainMenuBar()) {

            //mManager->setMenuHeight(ImGui::GetWindowSize().y);

            if (ImGui::BeginMenu("Project")) {
                ImGui::EndMenu();
            }

            for (ToolBase *tool : safeIterate(uniquePtrToPtr(mCollector))) {
                tool->renderMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (ImGui::BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, ImGui::GetFrameHeight(), ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar)) {
            if (ImGui::BeginMenuBar()) {
                for (ToolBase *tool : safeIterate(uniquePtrToPtr(mCollector))) {
                    tool->renderStatus();
                }
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }

        finishToolRead();

        for (ToolBase *tool : uniquePtrToPtr(mCollector)) {
            tool->update();
        }

        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGui::End();
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            ImGui::Render();
        else
            ImGui::EndFrame();

        ImGui::UpdatePlatformWindows();
    }

    const std::vector<std::unique_ptr<ToolBase>> &ImRoot::tools()
    {
        return mCollector;
    }

    ToolBase &ImRoot::getToolComponent(size_t index)
    {
        return mCollector.get(index);
    }

    ImGuiDockNode *ImRoot::dockNode() const
    {
        return mDockNode;
    }

    void ImRoot::finishToolRead()
    {
        if (mToolReadTool) {

            auto buf = std::make_unique<std::stringbuf>(mToolReadBuffer.str());
            Serialize::FormattedSerializeStream in { std::make_unique<Serialize::IniFormatter>(), { std::move(buf) } };

            Serialize::StreamResult result = Serialize::read(in, *mToolReadTool, nullptr, {}, Serialize::StateTransmissionFlags_SkipId);
            if (result.mState != Serialize::StreamState::OK) {
                LOG_ERROR(result);
            }

            mToolReadTool = nullptr;
        }
    }

}
}
