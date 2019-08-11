#include "../toolslib.h"

#include "imguimanager.h"

#include "Madgine/app/application.h"
#include "gui/guisystem.h"
#include "gui/widgets/toplevelwindow.h"
#include "input/inputhandler.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "input/inputevents.h"

#include "Modules/math/vector3.h"

#include "Modules/debug/profiler/profiler.h"

#include "gui/widgets/toolwindow.h"

#include "Interfaces/window/windowapi.h"

namespace Engine {
namespace Tools {

    static void CreateImGuiToolWindow(ImGuiViewport *vp)
    {

        ImGuiIO &io = ImGui::GetIO();
        GUI::TopLevelWindow *topLevel = static_cast<GUI::TopLevelWindow *>(io.BackendPlatformUserData);

        Window::WindowSettings settings;
        settings.mHeadless = true;
        settings.mHidden = true;
        GUI::ToolWindow *window = topLevel->createToolWindow(settings);
        vp->PlatformUserData = window;
        vp->PlatformHandle = window->window();
        vp->PlatformHandleRaw = reinterpret_cast<void *>(window->window()->mHandle);
    }
    static void DestroyImGuiToolWindow(ImGuiViewport *vp)
    {
        ImGuiIO &io = ImGui::GetIO();
        GUI::TopLevelWindow *topLevel = static_cast<GUI::TopLevelWindow *>(io.BackendPlatformUserData);

        GUI::ToolWindow *toolWindow = static_cast<GUI::ToolWindow *>(vp->PlatformUserData);
        vp->PlatformUserData = nullptr;
        vp->PlatformHandle = nullptr;
        vp->PlatformHandleRaw = nullptr;
        topLevel->destroyToolWindow(toolWindow);
    }
    static void ShowImGuiToolWindow(ImGuiViewport *vp)
    {
        Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
        w->show();
    }
    static void SetImGuiToolWindowPos(ImGuiViewport *vp, ImVec2 pos)
    {
        Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
        w->setRenderPos(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y));
    }
    static ImVec2 GetImGuiToolWindowPos(ImGuiViewport *vp)
    {
        Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
        return { static_cast<float>(w->renderX()), static_cast<float>(w->renderY()) };
    }
    static void SetImGuiToolWindowSize(ImGuiViewport *vp, ImVec2 size)
    {
        Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
        w->setRenderSize(static_cast<size_t>(size.x), static_cast<size_t>(size.y));
    }
    static ImVec2 GetImGuiToolWindowSize(ImGuiViewport *vp)
    {
        Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
        return { static_cast<float>(w->renderWidth()), static_cast<float>(w->renderHeight()) };
    }
    static void SetImGuiToolWindowFocus(ImGuiViewport *vp)
    {
        Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
        w->focus();
    }
    static bool GetImGuiToolWindowFocus(ImGuiViewport *vp)
    {
        Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
        return w->hasFocus();
    }
    static bool GetImGuiToolWindowMinimized(ImGuiViewport *vp)
    {
        Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
        return w->isMinimized();
    }
    static void SetImGuiToolWindowTitle(ImGuiViewport *vp, const char *title)
    {
        Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
        w->setTitle(title);
    }
    static void RenderImGuiToolWindow(ImGuiViewport *vp, void *render_arg)
    {
        GUI::ToolWindow *toolWindow = static_cast<GUI::ToolWindow *>(vp->PlatformUserData);
        toolWindow->beginFrame();
    }
    static void SwapImGuiToolWindowBuffers(ImGuiViewport *vp, void *render_arg)
    {
        GUI::ToolWindow *toolWindow = static_cast<GUI::ToolWindow *>(vp->PlatformUserData);
        toolWindow->endFrame();
    }

    ImGuiManager::ImGuiManager(Engine::App::Application &app)
        : mApp(app)
    {
        GUI::TopLevelWindow &window = *app.getGlobalAPIComponent<GUI::GUISystem>().topLevelWindows().front();
        window.addOverlay(this);
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        if (Window::platformCapabilities.mSupportMultipleWindows) {
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

#if ANDROID
            io.DisplayFramebufferScale = ImVec2(3.0f, 3.0f);
#endif

            ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
            platform_io.Platform_CreateWindow = CreateImGuiToolWindow;
            platform_io.Platform_DestroyWindow = DestroyImGuiToolWindow;
            platform_io.Platform_ShowWindow = ShowImGuiToolWindow;
            platform_io.Platform_SetWindowPos = SetImGuiToolWindowPos;
            platform_io.Platform_GetWindowPos = GetImGuiToolWindowPos;
            platform_io.Platform_SetWindowSize = SetImGuiToolWindowSize;
            platform_io.Platform_GetWindowSize = GetImGuiToolWindowSize;
            platform_io.Platform_SetWindowFocus = SetImGuiToolWindowFocus;
            platform_io.Platform_GetWindowFocus = GetImGuiToolWindowFocus;
            platform_io.Platform_GetWindowMinimized = GetImGuiToolWindowMinimized;
            platform_io.Platform_SetWindowTitle = SetImGuiToolWindowTitle;
            platform_io.Platform_RenderWindow = RenderImGuiToolWindow;
            platform_io.Platform_SwapBuffers = SwapImGuiToolWindowBuffers;

            io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports | ImGuiBackendFlags_PlatformHasViewports;

            platform_io.Monitors.clear();
            for (Window::MonitorInfo info : Window::listMonitors()) {
                ImGuiPlatformMonitor monitor;
                monitor.MainPos = monitor.WorkPos = ImVec2 { static_cast<float>(info.x), static_cast<float>(info.y) };
                monitor.MainSize = monitor.WorkSize = ImVec2 { static_cast<float>(info.width), static_cast<float>(info.height) };
                platform_io.Monitors.push_back(monitor);
            }

            ImGuiViewport *main_viewport = ImGui::GetMainViewport();
            main_viewport->PlatformHandle = window.window();
        }
    }

    ImGuiManager::~ImGuiManager()
    {
        ImGui::DestroyContext();
    }

    void ImGuiManager::render()
    {
        PROFILE();

        ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        main_viewport->Flags |= ImGuiViewportFlags_NoRendererClear; //TODO: Is that necessary every Frame?

        ImGui::Render();

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();

            ImGui::GetPlatformIO().Renderer_RenderWindow(ImGui::GetMainViewport(), nullptr);

            ImGui::RenderPlatformWindowsDefault();
        }
    }

    bool ImGuiManager::injectKeyPress(const Engine::Input::KeyEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.KeysDown[arg.scancode] = true;

        io.AddInputCharacter(arg.text);

        return io.WantCaptureKeyboard;
    }

    bool ImGuiManager::injectKeyRelease(const Engine::Input::KeyEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.KeysDown[arg.scancode] = false;

        return io.WantCaptureKeyboard;
    }

    bool ImGuiManager::injectPointerPress(const Engine::Input::PointerEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[arg.button - 1] = true;

        return io.WantCaptureMouse;
    }

    bool ImGuiManager::injectPointerRelease(const Engine::Input::PointerEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[arg.button - 1] = false;

        return io.WantCaptureMouse;
    }

    bool ImGuiManager::injectPointerMove(const Engine::Input::PointerEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.MousePos.x = arg.position.x / io.DisplayFramebufferScale.x;
        io.MousePos.y = arg.position.y / io.DisplayFramebufferScale.y;

        //LOG(io.MousePos.x << ", " << io.MousePos.y);

        io.MouseWheel += arg.scrollWheel;

        return io.WantCaptureMouse;
    }

    void ImGuiManager::calculateAvailableScreenSpace(Window::Window *w, Vector3 &pos, Vector3 &size)
    {
        pos.x += mAreaPos.x - w->renderX();
        pos.y += mAreaPos.y - w->renderY();
        if (mAreaSize != Vector2::ZERO) {
            size.x = mAreaSize.x;
            size.y = mAreaSize.y;
        } else {
            size.x -= mAreaPos.x;
            size.y -= mAreaPos.y;
        }
    }

    void ImGuiManager::setMenuHeight(float h)
    {
        if (mAreaPos.x == 0.0f)
            mAreaPos.y = h;
    }

    void ImGuiManager::setCentralNode(ImGuiDockNode *node)
    {
        if (node) {
            mAreaPos = Vector2 { node->Pos };
            mAreaSize = Vector2 { node->Size };
        } else {
            mAreaPos = Vector2::ZERO;
            mAreaSize = Vector2::ZERO;
        }
    }

}
}