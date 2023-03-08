#include "../clienttoolslib.h"

#include "clientimroot.h"

#include "Interfaces/window/windowapi.h"
#include "Interfaces/window/windowsettings.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "im3d/im3d.h"

#include "Meta/serialize/serializetable_impl.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendertarget.h"

#include "Modules/debug/profiler/profile.h"

#include "Madgine/window/toolwindow.h"

#include "Madgine/window/mainwindow.h"

#include "Interfaces/input/inputevents.h"

#include "Interfaces/filesystem/fsapi.h"

#include "Madgine_Tools/toolbase.h"

#include "Madgine/fontloader/fontloader.h"

#include "Madgine/resources/resourcemanager.h"

#include "imgui/misc/freetype/imgui_freetype.h"

#include "imgui/imguiaddons.h"

#include "Madgine_Tools/imguiicons.h"

METATABLE_BEGIN_BASE(Engine::Tools::ClientImRoot, Engine::Tools::ImRoot)
METATABLE_END(Engine::Tools::ClientImRoot)

SERIALIZETABLE_BEGIN(Engine::Tools::ClientImRoot)
SERIALIZETABLE_END(Engine::Tools::ClientImRoot)

VIRTUALUNIQUECOMPONENTBASE(Engine::Tools::ClientImRoot)

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
        w->setRenderPos({ static_cast<int>(pos.x * io.DisplayFramebufferScale.x), static_cast<int>(pos.y * io.DisplayFramebufferScale.y) });
    }
    static ImVec2 GetImGuiToolWindowPos(ImGuiViewport *vp)
    {
        ImGuiIO &io = ImGui::GetIO();
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        return { static_cast<float>(w->renderPos().x / io.DisplayFramebufferScale.x), static_cast<float>(w->renderPos().y / io.DisplayFramebufferScale.y) };
    }
    static void SetImGuiToolWindowSize(ImGuiViewport *vp, ImVec2 size)
    {
        ImGuiIO &io = ImGui::GetIO();
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        w->setRenderSize({ static_cast<int>(size.x * io.DisplayFramebufferScale.x), static_cast<int>(size.y * io.DisplayFramebufferScale.y) });
    }
    static ImVec2 GetImGuiToolWindowSize(ImGuiViewport *vp)
    {
        ImGuiIO &io = ImGui::GetIO();
        Window::OSWindow *w = static_cast<Window::OSWindow *>(vp->PlatformHandle);
        return { static_cast<float>(w->renderSize().x / io.DisplayFramebufferScale.x), static_cast<float>(w->renderSize().y / io.DisplayFramebufferScale.y) };
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
        : VirtualData(window, 80)
        , mImGuiIniFilePath(Filesystem::appDataPath() / "imgui.ini")
    {
    }

    ClientImRoot::~ClientImRoot()
    {
    }

    Threading::Task<bool> ClientImRoot::init()
    {
        if (!co_await MainWindowComponentBase::init())
            co_return false;

        ImGui::CreateContext();
        Im3D::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.UserData = this;

        io.IniFilename = mImGuiIniFilePath.c_str();

        io.DisplayFramebufferScale = ImVec2 { Window::platformCapabilities.mScalingFactor, Window::platformCapabilities.mScalingFactor };

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
                monitor.MainPos = monitor.WorkPos = ImVec2 { static_cast<float>(info.mPosition.x), static_cast<float>(info.mPosition.y) };
                monitor.MainSize = monitor.WorkSize = ImVec2 { static_cast<float>(info.mSize.x), static_cast<float>(info.mSize.y) };
                platform_io.Monitors.push_back(monitor);
            }

            ImGuiViewport *main_viewport = ImGui::GetMainViewport();
            main_viewport->PlatformHandle = mWindow.osWindow();
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

        static const ImWchar icons_ranges[] = { 0xf100, 0xf1ff, 0 };

        ImFontConfig config;
        config.MergeMode = true;
        config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
        //config.GlyphMinAdvanceX = 13.0f;
        config.GlyphOffset = { 0.0f, 3.0f };

        io.Fonts->AddFontDefault();
        io.Fonts->AddFontFromFileTTF(Engine::Resources::ResourceManager::getSingleton().findResourceFile("icons.ttf").c_str(), 13, &config, icons_ranges);
        io.Fonts->Build();

        Im3D::GetIO().mFetchFont = [](const char *fontName) {
            Render::FontLoader::Handle font;
            font.load(fontName);
            font.info()->setPersistent(true);

            if (font.available()) {
                return Im3DFont {
                    (Im3DTextureId)font->mTexture->mTextureHandle,
                    font->mTextureSize,
                    font->mGlyphs.data()
                };
            } else {
                return Im3DFont {};
            }
        };

        ImGui::FilesystemPickerOptions *filepickerOptions = ImGui::GetFilesystemPickerOptions();

        filepickerOptions->mIconLookup = [](const Filesystem::Path &path, bool isDir) {
            if (isDir)
                return IMGUI_ICON_FOLDER " ";
            else
                return IMGUI_ICON_FILE " ";
        };

        if (!co_await ImRoot::init())
            co_return false;

        co_return true;
    }

    Threading::Task<void> ClientImRoot::finalize()
    {
        ImGui::SaveIniSettingsToDisk(ImGui::GetIO().IniFilename);

        ImGui::GetIO().IniFilename = nullptr;

        co_await ImRoot::finalize();

        if (Window::platformCapabilities.mSupportMultipleWindows) {
            ImGuiViewport *main_viewport = ImGui::GetMainViewport();
            main_viewport->PlatformHandle = nullptr;
        }

        Im3D::DestroyContext();
        ImGui::DestroyContext();

        co_await MainWindowComponentBase::finalize();

        co_return;
    }

    void ClientImRoot::addRenderTarget(Render::RenderTarget *target)
    {
        addDependency(target);
    }

    void ClientImRoot::removeRenderTarget(Render::RenderTarget *target)
    {
        removeDependency(target);
    }

    static Input::CursorIcon convertCursorIcon(ImGuiMouseCursor cursor)
    {
#define HELPER(x)              \
    case ImGuiMouseCursor_##x: \
        return Input::CursorIcon::x;
        switch (cursor) {
            HELPER(Arrow)
            HELPER(TextInput)
            HELPER(ResizeAll)
            HELPER(ResizeNS)
            HELPER(ResizeEW)
            HELPER(ResizeNESW)
            HELPER(ResizeNWSE)
            HELPER(Hand)
            HELPER(NotAllowed)
        default:
            throw 0;
        }
    }

    void ClientImRoot::render(Render::RenderTarget *target, size_t iteration)
    {
        PROFILE();

        target->context()->pushAnnotation("ImGui");

        if (mWindow.getRenderWindow() == target) {

            MainWindowComponentBase::render(target, iteration);

            ImGuiIO &io = ImGui::GetIO();

            io.MouseWheel += mZAxis * 0.3f;

            io.DeltaTime = mFrameClock.tick<std::chrono::duration<float>>().count();

            io.BackendPlatformUserData = &mWindow;

            Vector2i size = mWindow.getScreenSpace().mSize;

            io.DisplaySize = ImVec2(size.x / io.DisplayFramebufferScale.x, size.y / io.DisplayFramebufferScale.y);

            mWindow.osWindow()->setCursorIcon(convertCursorIcon(ImGui::GetMouseCursor()));

            newFrame();

            ImRoot::render();

            setCentralNode();

            ImGuiViewport *main_viewport = ImGui::GetMainViewport();
            main_viewport->Flags |= ImGuiViewportFlags_NoRendererClear; //TODO: Is that necessary every Frame?

            if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)) {
                ImGui::Render();
            }

            renderViewport(main_viewport);
        } else {
            renderViewport(mViewportMappings.at(target));
        }

        target->context()->popAnnotation();
    }

    void ClientImRoot::addViewportMapping(Render::RenderTarget *target, ImGuiViewport *vp)
    {
        mViewportMappings[target] = vp;
    }

    void ClientImRoot::removeViewportMapping(Render::RenderTarget *target)
    {
        mViewportMappings.erase(target);
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

    bool ClientImRoot::includeInLayout() const
    {
        return false;
    }

    bool ClientImRoot::injectKeyPress(const Engine::Input::KeyEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.KeysDown[arg.scancode] = true;

        io.AddInputCharacter(arg.text);

        io.KeyShift = arg.mControlKeys.mShift;
        io.KeyCtrl = arg.mControlKeys.mCtrl;
        io.KeyAlt = arg.mControlKeys.mAlt;

        return io.WantCaptureKeyboard;
    }

    bool ClientImRoot::injectKeyRelease(const Engine::Input::KeyEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.KeysDown[arg.scancode] = false;

        io.KeyShift = arg.mControlKeys.mShift;
        io.KeyCtrl = arg.mControlKeys.mCtrl;
        io.KeyAlt = arg.mControlKeys.mAlt;

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
            io.MousePos = Vector2 { static_cast<float>(arg.screenPosition.x), static_cast<float>(arg.screenPosition.y) } / io.DisplayFramebufferScale;
        else
            io.MousePos = Vector2 { static_cast<float>(arg.windowPosition.x), static_cast<float>(arg.windowPosition.y) } / io.DisplayFramebufferScale;

        //LOG(io.MousePos.x << ", " << io.MousePos.y);

        //LOG(arg.scrollWheel);

        return io.WantCaptureMouse;
    }

    bool ClientImRoot::injectAxisEvent(const Engine::Input::AxisEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();
        switch (arg.mAxisType) {
        case Input::AxisEventArgs::WHEEL:
            io.MouseWheel += arg.mAxis1;
            break;
        case Input::AxisEventArgs::Z:
            mZAxis = arg.mAxis1;
            break;
        case Input::AxisEventArgs::LEFT:
            mLeftControllerStick = { arg.mAxis1, arg.mAxis2 };
            break;
        case Input::AxisEventArgs::RIGHT:
            mRightControllerStick = { arg.mAxis1, arg.mAxis2 };
            break;
        case Input::AxisEventArgs::DPAD:
            mDPadState = arg.mAxis1;
            break;
        }

        return io.WantCaptureMouse;
    }

    void ClientImRoot::setCentralNode()
    {
        ImGuiIO &io = ImGui::GetIO();

        Vector2 oldSize = mAreaSize;

        ImGuiDockNode *node = ImGui::DockBuilderGetCentralNode(mDockSpaceId);

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

    Threading::TaskQueue *ClientImRoot::taskQueue() const
    {
        return mWindow.taskQueue();
    }

}
}
