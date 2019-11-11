#include "../clienttoolslib.h"

#include "immanager.h"

#include "Madgine/app/application.h"
#include "Madgine/gui/widgets/toplevelwindow.h"
#include "Madgine/input/inputhandler.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "Madgine/input/inputevents.h"

#include "Modules/math/vector3.h"

#include "Modules/debug/profiler/profiler.h"

#include "Madgine/gui/widgets/toolwindow.h"

#include "Interfaces/window/windowapi.h"

#include "im3d/im3d.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Madgine/gui/widgets/toolwindow.h"

#include "Madgine/render/rendertarget.h"

DEFINE_UNIQUE_COMPONENT(Engine::Tools, ImManager)

METATABLE_BEGIN(Engine::Tools::ImManager)
METATABLE_END(Engine::Tools::ImManager)

RegisterType(Engine::Tools::ImManager)

    namespace Engine
{
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

            ImManager *manager = static_cast<ImManager *>(io.UserData);
            manager->addViewportMapping(window->getRenderer(), vp);
            window->getRenderer()->addRenderPass(manager);
        }
        static void DestroyImGuiToolWindow(ImGuiViewport *vp)
        {
            if (vp->PlatformUserData) {
                ImGuiIO &io = ImGui::GetIO();
                GUI::TopLevelWindow *topLevel = static_cast<GUI::TopLevelWindow *>(io.BackendPlatformUserData);

                GUI::ToolWindow *toolWindow = static_cast<GUI::ToolWindow *>(vp->PlatformUserData);
                vp->PlatformUserData = nullptr;
                vp->PlatformHandle = nullptr;
                vp->PlatformHandleRaw = nullptr;
                topLevel->destroyToolWindow(toolWindow);

                static_cast<ImManager *>(io.UserData)->removeViewportMapping(toolWindow->getRenderer());
            }
        }
        static void ShowImGuiToolWindow(ImGuiViewport *vp)
        {
            Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
            w->show();
        }
        static void SetImGuiToolWindowPos(ImGuiViewport *vp, ImVec2 pos)
        {
            ImGuiIO &io = ImGui::GetIO();
            Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
            w->setRenderPos(static_cast<size_t>(pos.x * io.DisplayFramebufferScale.x), static_cast<size_t>(pos.y * io.DisplayFramebufferScale.y));
        }
        static ImVec2 GetImGuiToolWindowPos(ImGuiViewport *vp)
        {
            ImGuiIO &io = ImGui::GetIO();
            Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
            return { static_cast<float>(w->renderX() / io.DisplayFramebufferScale.x), static_cast<float>(w->renderY() / io.DisplayFramebufferScale.y) };
        }
        static void SetImGuiToolWindowSize(ImGuiViewport *vp, ImVec2 size)
        {
            ImGuiIO &io = ImGui::GetIO();
            Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
            w->setRenderSize(static_cast<size_t>(size.x * io.DisplayFramebufferScale.x), static_cast<size_t>(size.y * io.DisplayFramebufferScale.y));
        }
        static ImVec2 GetImGuiToolWindowSize(ImGuiViewport *vp)
        {
            ImGuiIO &io = ImGui::GetIO();
            Window::Window *w = static_cast<Window::Window *>(vp->PlatformHandle);
            return { static_cast<float>(w->renderWidth() / io.DisplayFramebufferScale.x), static_cast<float>(w->renderHeight() / io.DisplayFramebufferScale.y) };
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
            //toolWindow->beginFrame();
        }
        static void SwapImGuiToolWindowBuffers(ImGuiViewport *vp, void *render_arg)
        {
            GUI::ToolWindow *toolWindow = static_cast<GUI::ToolWindow *>(vp->PlatformUserData);
            //toolWindow->endFrame();
        }

        ImManager::ImManager(GUI::TopLevelWindow &window)
            : mWindow(window)
        {
            ImGui::CreateContext();
            Im3D::CreateContext();

            ImGuiIO &io = ImGui::GetIO();
            io.UserData = this;

#if ANDROID
            io.DisplayFramebufferScale = ImVec2(3.0f, 3.0f);
#endif

            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            if (Window::platformCapabilities.mSupportMultipleWindows) {
                io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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
                main_viewport->PlatformHandle = mWindow.window();
            }

            //Input
            io.KeyMap[ImGuiKey_Tab] = Input::KC_TAB;
            io.KeyMap[ImGuiKey_LeftArrow] = Input::KC_LEFT;
            io.KeyMap[ImGuiKey_RightArrow] = Input::KC_RIGHT;
            io.KeyMap[ImGuiKey_UpArrow] = Input::KC_UP;
            io.KeyMap[ImGuiKey_DownArrow] = Input::KC_DOWN;
            io.KeyMap[ImGuiKey_PageUp] = Input::KC_PGUP;
            io.KeyMap[ImGuiKey_PageDown] = Input::KC_PGDOWN;
            io.KeyMap[ImGuiKey_Home] = Input::KC_HOME;
            io.KeyMap[ImGuiKey_End] = Input::KC_END;
            io.KeyMap[ImGuiKey_Insert] = Input::KC_INSERT;
            io.KeyMap[ImGuiKey_Delete] = Input::KC_DELETE;
            io.KeyMap[ImGuiKey_Backspace] = Input::KC_BACK;
            io.KeyMap[ImGuiKey_Space] = Input::KC_SPACE;
            io.KeyMap[ImGuiKey_Enter] = Input::KC_RETURN;
            io.KeyMap[ImGuiKey_Escape] = Input::KC_ESCAPE;
            io.KeyMap[ImGuiKey_KeyPadEnter] = Input::KC_NUMPADENTER;
            io.KeyMap[ImGuiKey_A] = Input::KC_A;
            io.KeyMap[ImGuiKey_C] = Input::KC_C;
            io.KeyMap[ImGuiKey_V] = Input::KC_V;
            io.KeyMap[ImGuiKey_X] = Input::KC_X;
            io.KeyMap[ImGuiKey_Y] = Input::KC_Y;
            io.KeyMap[ImGuiKey_Z] = Input::KC_Z;
        }

        ImManager::~ImManager()
        {
            Im3D::DestroyContext();
            ImGui::DestroyContext();
        }

        void ImManager::render(Render::RenderTarget *target)
        {
            PROFILE();

            if (mWindow.getRenderWindow() == target) {

                ImGuiViewport *main_viewport = ImGui::GetMainViewport();
                main_viewport->Flags |= ImGuiViewportFlags_NoRendererClear; //TODO: Is that necessary every Frame?

                if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                    ImGui::UpdatePlatformWindows();

                    ImGui::GetPlatformIO().Renderer_RenderWindow(ImGui::GetMainViewport(), nullptr);
                }
            } else {
                ImGui::GetPlatformIO().Renderer_RenderWindow(mViewportMappings.at(target), nullptr);
            }
        }

        void ImManager::addViewportMapping(Render::RenderTarget *target, ImGuiViewport *vp)
        {
            mViewportMappings[target] = vp;
        }

        void ImManager::removeViewportMapping(Render::RenderTarget *target)
        {
            mViewportMappings.erase(target);
        }

    }
}
