#include "../clienttoolslib.h"

#include "clientimroot.h"

#include "Interfaces/window/windowapi.h"
#include "Interfaces/window/windowsettings.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "im3d/im3d.h"

#include "Modules/serialize/serializetable_impl.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Madgine/render/rendertarget.h"

#include "Modules/debug/profiler/profile.h"

#include "Madgine/window/toolwindow.h"

#include "Madgine/window/mainwindow.h"

#include "Interfaces/input/inputevents.h"

#include "Interfaces/filesystem/api.h"

//UNIQUECOMPONENT(Engine::Tools::ClientImRoot);

METATABLE_BEGIN(Engine::Tools::ClientImRoot)
MEMBER(mRoot)
METATABLE_END(Engine::Tools::ClientImRoot)

SERIALIZETABLE_BEGIN(Engine::Tools::ClientImRoot)
SERIALIZETABLE_END(Engine::Tools::ClientImRoot)

namespace Engine {
namespace Tools {

    static void CreateImGuiToolWindow(ImGuiViewport *vp)
    {

        ImGuiIO &io = ImGui::GetIO();
        Window::MainWindow *topLevel = static_cast<Window::MainWindow *>(io.BackendPlatformUserData);

        Window::WindowSettings settings;
        settings.mHeadless = true;
        settings.mHidden = true;
        Window::ToolWindow *window = topLevel->createToolWindow(settings);
        vp->PlatformUserData = window;
        vp->PlatformHandle = window->osWindow();
        vp->PlatformHandleRaw = reinterpret_cast<void *>(window->osWindow()->mHandle);

        ClientImRoot *root = static_cast<ClientImRoot *>(io.UserData);
        root->addViewportMapping(window->getRenderer(), vp);
        window->getRenderer()->addRenderPass(root);
        vp->RendererUserData = window->getRenderer();
    }
    static void DestroyImGuiToolWindow(ImGuiViewport *vp)
    {
        if (vp->PlatformUserData) {
            ImGuiIO &io = ImGui::GetIO();
            Window::MainWindow *topLevel = static_cast<Window::MainWindow *>(io.BackendPlatformUserData);

            Window::ToolWindow *toolWindow = static_cast<Window::ToolWindow *>(vp->PlatformUserData);
            vp->PlatformUserData = nullptr;
            vp->PlatformHandle = nullptr;
            vp->PlatformHandleRaw = nullptr;
            topLevel->destroyToolWindow(toolWindow);

            vp->RendererUserData = nullptr;
            static_cast<ClientImRoot *>(io.UserData)->removeViewportMapping(toolWindow->getRenderer());
        }
    }
    static void ShowImGuiToolWindow(ImGuiViewport *vp)
    {
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        w->show();
    }
    static void SetImGuiToolWindowPos(ImGuiViewport *vp, ImVec2 pos)
    {
        ImGuiIO &io = ImGui::GetIO();
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        w->setRenderPos(static_cast<size_t>(pos.x * io.DisplayFramebufferScale.x), static_cast<size_t>(pos.y * io.DisplayFramebufferScale.y));
    }
    static ImVec2 GetImGuiToolWindowPos(ImGuiViewport *vp)
    {
        ImGuiIO &io = ImGui::GetIO();
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        return { static_cast<float>(w->renderX() / io.DisplayFramebufferScale.x), static_cast<float>(w->renderY() / io.DisplayFramebufferScale.y) };
    }
    static void SetImGuiToolWindowSize(ImGuiViewport *vp, ImVec2 size)
    {
        ImGuiIO &io = ImGui::GetIO();
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        w->setRenderSize(static_cast<size_t>(size.x * io.DisplayFramebufferScale.x), static_cast<size_t>(size.y * io.DisplayFramebufferScale.y));
    }
    static ImVec2 GetImGuiToolWindowSize(ImGuiViewport *vp)
    {
        ImGuiIO &io = ImGui::GetIO();
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        return { static_cast<float>(w->renderWidth() / io.DisplayFramebufferScale.x), static_cast<float>(w->renderHeight() / io.DisplayFramebufferScale.y) };
    }
    static void SetImGuiToolWindowFocus(ImGuiViewport *vp)
    {
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        w->focus();
    }
    static bool GetImGuiToolWindowFocus(ImGuiViewport *vp)
    {
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        return w->hasFocus();
    }
    static bool GetImGuiToolWindowMinimized(ImGuiViewport *vp)
    {
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        return w->isMinimized();
    }
    static void SetImGuiToolWindowTitle(ImGuiViewport *vp, const char *title)
    {
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        w->setTitle(title);
    }

    ClientImRoot::ClientImRoot(Window::MainWindow &window)
        : SerializableUnit(window, 80)
        , mRoot(this)
        , mImGuiIniFilePath(Filesystem::appDataPath() / "imgui.ini")
    {
    }

    ClientImRoot::~ClientImRoot()
    {
    }

    bool ClientImRoot::init()
    {
        ImGui::CreateContext();
        Im3D::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.UserData = this;

        io.IniFilename = mImGuiIniFilePath.c_str();

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
            /*platform_io.Platform_RenderWindow = RenderImGuiToolWindow;
                platform_io.Platform_SwapBuffers = SwapImGuiToolWindowBuffers;*/

            io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports | ImGuiBackendFlags_PlatformHasViewports;

            platform_io.Monitors.clear();
            for (Window::MonitorInfo info : Window::listMonitors()) {
                ImGuiPlatformMonitor monitor;
                monitor.MainPos = monitor.WorkPos = ImVec2 { static_cast<float>(info.x), static_cast<float>(info.y) };
                monitor.MainSize = monitor.WorkSize = ImVec2 { static_cast<float>(info.width), static_cast<float>(info.height) };
                platform_io.Monitors.push_back(monitor);
            }

            ImGuiViewport *main_viewport = ImGui::GetMainViewport();
            main_viewport->PlatformHandle = mWindow.osWindow();
            main_viewport->RendererUserData = mWindow.getRenderWindow();
        }

        //Input
        io.KeyMap[ImGuiKey_Tab] = Input::Key::Tabulator;
        io.KeyMap[ImGuiKey_LeftArrow] = Input::Key::LeftArrow;
        io.KeyMap[ImGuiKey_RightArrow] = Input::Key::RightArrow;
        io.KeyMap[ImGuiKey_UpArrow] = Input::Key::UpArrow;
        io.KeyMap[ImGuiKey_DownArrow] = Input::Key::DownArrow;
        io.KeyMap[ImGuiKey_PageUp] = Input::Key::PageUp;
        io.KeyMap[ImGuiKey_PageDown] = Input::Key::PageDown;
        io.KeyMap[ImGuiKey_Home] = Input::Key::Home;
        io.KeyMap[ImGuiKey_End] = Input::Key::End;
        io.KeyMap[ImGuiKey_Insert] = Input::Key::Insert;
        io.KeyMap[ImGuiKey_Delete] = Input::Key::Delete;
        io.KeyMap[ImGuiKey_Backspace] = Input::Key::Backspace;
        io.KeyMap[ImGuiKey_Space] = Input::Key::Space;
        io.KeyMap[ImGuiKey_Enter] = Input::Key::Return;
        io.KeyMap[ImGuiKey_Escape] = Input::Key::Escape;
        io.KeyMap[ImGuiKey_KeyPadEnter] = Input::Key::Return;
        io.KeyMap[ImGuiKey_A] = Input::Key::A;
        io.KeyMap[ImGuiKey_C] = Input::Key::C;
        io.KeyMap[ImGuiKey_V] = Input::Key::V;
        io.KeyMap[ImGuiKey_X] = Input::Key::X;
        io.KeyMap[ImGuiKey_Y] = Input::Key::Y;
        io.KeyMap[ImGuiKey_Z] = Input::Key::Z;

        if (!mRoot.callInit())
            return false;

        mWindow.addFrameListener(this);

        mWindow.getRenderWindow()->addRenderPass(this);

        return true;
    }

    void ClientImRoot::finalize()
    {
        mWindow.getRenderWindow()->removeRenderPass(this);

        mWindow.removeFrameListener(this);

        mRoot.callFinalize();

        if (Window::platformCapabilities.mSupportMultipleWindows) {
            ImGuiViewport *main_viewport = ImGui::GetMainViewport();
            main_viewport->PlatformHandle = nullptr;
            main_viewport->RendererUserData = nullptr;
        }

        Im3D::DestroyContext();
        ImGui::DestroyContext();
    }

    void ClientImRoot::render(Render::RenderTarget *target)
    {
        PROFILE();

        if (mWindow.getRenderWindow() == target) {

            ImGuiViewport *main_viewport = ImGui::GetMainViewport();
            main_viewport->Flags |= ImGuiViewportFlags_NoRendererClear; //TODO: Is that necessary every Frame?

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();

                ImGui::GetPlatformIO().Renderer_RenderWindow(ImGui::GetMainViewport(), nullptr);
            } else {
                ImGui::Render();
            }
        } else {
            ImGui::GetPlatformIO().Renderer_RenderWindow(mViewportMappings.at(target), nullptr);
        }
    }

    void ClientImRoot::addViewportMapping(Render::RenderTarget *target, ImGuiViewport *vp)
    {
        mViewportMappings[target] = vp;
    }

    void ClientImRoot::removeViewportMapping(Render::RenderTarget *target)
    {
        mViewportMappings.erase(target);
    }

    int ClientImRoot::priority() const
    {
        return 100;
    }

    bool ClientImRoot::frameStarted(std::chrono::microseconds timeSinceLastFrame)
    {
        newFrame((float)timeSinceLastFrame.count() / 1000000.0f);

        return true;
    }

    bool ClientImRoot::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context)
    {
        mRoot.frame();

        setCentralNode(mRoot.dockNode());

        return true;
    }

    std::string_view ClientImRoot::key() const
    {
        return "ClientImRoot";
    }

    Rect2i ClientImRoot::getChildClientSpace()
    {
        ImGuiIO &io = ImGui::GetIO();
        if (mAreaSize == Vector2 { 0, 0 })
            return mClientSpace;
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            return { (mAreaPos - getScreenSpace().mTopLeft).floor(), mAreaSize.floor() };
        else
            return { mAreaPos.floor(), mAreaSize.floor() };
    }

    bool ClientImRoot::injectKeyPress(const Engine::Input::KeyEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.KeysDown[arg.scancode] = true;

        io.KeyShift = mWindow.osWindow()->isKeyDown(Input::Key::LShift) || mWindow.osWindow()->isKeyDown(Input::Key::RShift);
        io.KeyCtrl = mWindow.osWindow()->isKeyDown(Input::Key::LControl) || mWindow.osWindow()->isKeyDown(Input::Key::RControl);
        io.KeyAlt = mWindow.osWindow()->isKeyDown(Input::Key::LAlt) || mWindow.osWindow()->isKeyDown(Input::Key::RAlt);

        io.AddInputCharacter(arg.text);

        return io.WantCaptureKeyboard;
    }

    bool ClientImRoot::injectKeyRelease(const Engine::Input::KeyEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.KeysDown[arg.scancode] = false;

        io.KeyShift = mWindow.osWindow()->isKeyDown(Input::Key::LShift) || mWindow.osWindow()->isKeyDown(Input::Key::RShift);
        io.KeyCtrl = mWindow.osWindow()->isKeyDown(Input::Key::LControl) || mWindow.osWindow()->isKeyDown(Input::Key::RControl);
        io.KeyAlt = mWindow.osWindow()->isKeyDown(Input::Key::LAlt) || mWindow.osWindow()->isKeyDown(Input::Key::RAlt);

        return io.WantCaptureKeyboard;
    }

    bool ClientImRoot::injectPointerPress(const Engine::Input::PointerEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[arg.button - 1] = true;

        return io.WantCaptureMouse;
    }

    bool ClientImRoot::injectPointerRelease(const Engine::Input::PointerEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[arg.button - 1] = false;

        return io.WantCaptureMouse;
    }

    bool ClientImRoot::injectPointerMove(const Engine::Input::PointerEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            io.MousePos = Vector2 { arg.screenPosition } / io.DisplayFramebufferScale;
        else
            io.MousePos = Vector2 { arg.windowPosition } / io.DisplayFramebufferScale;

        //LOG(io.MousePos.x << ", " << io.MousePos.y);

        io.MouseWheel += arg.scrollWheel;

        //LOG(arg.scrollWheel);

        return io.WantCaptureMouse;
    }

    void ClientImRoot::setMenuHeight(float h)
    {
        ImGuiIO &io = ImGui::GetIO();

        if (mAreaPos.x == 0.0f)
            mAreaPos.y = h * io.DisplayFramebufferScale.y;
    }

    void ClientImRoot::setCentralNode(ImGuiDockNode *node)
    {
        ImGuiIO &io = ImGui::GetIO();

        Vector2 oldSize = mAreaSize;

        if (node) {
            mAreaPos = Vector2 { node->Pos } * io.DisplayFramebufferScale;
            mAreaSize = Vector2 { node->Size } * io.DisplayFramebufferScale;
        } else {
            mAreaPos = Vector2::ZERO;
            mAreaSize = Vector2::ZERO;
        }

        if (mAreaSize != oldSize)
            mWindow.applyClientSpaceResize(this);
    }

}
}
