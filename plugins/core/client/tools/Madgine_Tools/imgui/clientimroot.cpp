#include "../clienttoolslib.h"

#include "clientimroot.h"

#include "Madgine/app/application.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

namespace Engine {
namespace Tools {

    ClientImRoot::ClientImRoot(GUI::TopLevelWindow &window)
        : UniqueComponent(window)
        , mRoot(this)
    {
    }

    bool ClientImRoot::init()
    {
        mManager.emplace(mWindow);
        assert(*mManager);

        if (!mRoot.callInit())
            return false;

        App::Application::getSingleton().addFrameListener(this);

        return true;
    }

    void ClientImRoot::finalize()
    {
        App::Application::getSingleton().removeFrameListener(this);

        mRoot.callFinalize();

        mManager.reset();
    }

    bool ClientImRoot::frameStarted(std::chrono::microseconds timeSinceLastFrame)
    {
        return true;
    }

    bool ClientImRoot::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
    {
        (*mManager)->newFrame((float)timeSinceLastFrame.count() / 1000000.0f);

        ImGuiID dockspace_id = ImGui::GetID("MadgineDockSpace");
        ImGuiDockNode *node = ImGui::DockBuilderGetNode(dockspace_id);

        if (node)
            (*mManager)->setCentralNode(node->CentralNode);

        return mRoot.frame();
    }

    const ImManager &ClientImRoot::manager() const
    {
        return **mManager;
    }

}
}

UNIQUECOMPONENT(Engine::Tools::ClientImRoot);

METATABLE_BEGIN(Engine::Tools::ClientImRoot)
MEMBER(mRoot)
METATABLE_END(Engine::Tools::ClientImRoot)

RegisterType(Engine::Tools::ClientImRoot);