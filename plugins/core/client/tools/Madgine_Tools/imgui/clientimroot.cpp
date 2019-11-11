#include "../clienttoolslib.h"

#include "clientimroot.h"

#include "Interfaces/window/windowapi.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "Madgine/render/rendertarget.h"

UNIQUECOMPONENT(Engine::Tools::ClientImRoot);

METATABLE_BEGIN(Engine::Tools::ClientImRoot)
MEMBER(mRoot)
METATABLE_END(Engine::Tools::ClientImRoot)

RegisterType(Engine::Tools::ClientImRoot);

namespace Engine {
namespace Tools {

    ClientImRoot::ClientImRoot(GUI::TopLevelWindow &window)
        : UniqueComponent(window, 80)
        , mRoot(this)
    {
    }

    bool ClientImRoot::init()
    {
        mManager.emplace(mWindow);
        assert(*mManager);

        if (!mRoot.callInit())
            return false;

        mWindow.addFrameListener(this);

		mWindow.getRenderWindow()->addRenderPass(mManager->get());

        return true;
    }

    void ClientImRoot::finalize()
    {
        mWindow.getRenderWindow()->removeRenderPass(mManager->get());

        mWindow.removeFrameListener(this);

        mRoot.callFinalize();

        mManager.reset();
    }

    bool ClientImRoot::frameStarted(std::chrono::microseconds timeSinceLastFrame)
    {
        (*mManager)->newFrame((float)timeSinceLastFrame.count() / 1000000.0f);

        return true;
    }

    bool ClientImRoot::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context)
    {
        setCentralNode(mRoot.dockNode());

        mRoot.frame();

        return true;
    }

    const char *ClientImRoot::key() const
    {
        return "ClientImRoot";
    }

    Rect2i ClientImRoot::getChildClientSpace()
    {
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			return { (mAreaPos - getScreenSpace().mTopLeft).floor(), mAreaSize.floor() };
        else
            return { mAreaPos.floor(), mAreaSize.floor() };
    }

    const ImManager &ClientImRoot::manager() const
    {
        return **mManager;
    }

	
    bool ClientImRoot::injectKeyPress(const Engine::Input::KeyEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.KeysDown[arg.scancode] = true;

        io.KeyShift = mWindow.input()->isKeyDown(Input::KC_LSHIFT) || mWindow.input()->isKeyDown(Input::KC_RSHIFT);
        io.KeyCtrl = mWindow.input()->isKeyDown(Input::KC_LCONTROL) || mWindow.input()->isKeyDown(Input::KC_RCONTROL);
        io.KeyAlt = mWindow.input()->isKeyDown(Input::KC_LMENU) || mWindow.input()->isKeyDown(Input::KC_RMENU);

        io.AddInputCharacter(arg.text);

        return io.WantCaptureKeyboard;
    }

    bool ClientImRoot::injectKeyRelease(const Engine::Input::KeyEventArgs &arg)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.KeysDown[arg.scancode] = false;

        io.KeyShift = mWindow.input()->isKeyDown(Input::KC_LSHIFT) || mWindow.input()->isKeyDown(Input::KC_RSHIFT);
        io.KeyCtrl = mWindow.input()->isKeyDown(Input::KC_LCONTROL) || mWindow.input()->isKeyDown(Input::KC_RCONTROL);
        io.KeyAlt = mWindow.input()->isKeyDown(Input::KC_LMENU) || mWindow.input()->isKeyDown(Input::KC_RMENU);

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

        io.MousePos = arg.position / io.DisplayFramebufferScale;

        LOG(io.MousePos.x << ", " << io.MousePos.y);

        io.MouseWheel += arg.scrollWheel;

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

        if (node) {
            mAreaPos = Vector2 { node->Pos } * io.DisplayFramebufferScale;
            mAreaSize = Vector2 { node->Size } * io.DisplayFramebufferScale;
        } else {
            mAreaPos = Vector2::ZERO;
            mAreaSize = Vector2::ZERO;
        }

		mWindow.applyClientSpaceResize(this);
    }

}
}
