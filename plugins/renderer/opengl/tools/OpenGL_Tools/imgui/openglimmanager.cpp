#include "../opengltoolslib.h"

#include "openglimmanager.h"

#include "imgui/imgui.h"
#include "imgui_impl_opengl3.h"

#include "OpenGL/openglrenderwindow.h"
#include "OpenGL/openglrendertexture.h"

#include "Modules/math/vector3.h"

#include "Madgine/app/application.h"
#include "Madgine/gui/toplevelwindow.h"

#include "im3d/im3d.h"

#include "OpenGL/openglfontloader.h"
#include "OpenGL/openglfontdata.h"

#include "Modules/keyvalue/metatable_impl.h"

namespace Engine {
namespace Tools {

    OpenGLImManager::OpenGLImManager(GUI::TopLevelWindow &window)
        : UniqueComponent(window)
    {
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplOpenGL3_CreateDeviceObjects();
        if (!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
            ImGui::GetIO().RenderDrawListsFn = ImGui_ImplOpenGL3_RenderDrawData;

        Im3D::GetIO().mFetchFont = [](const char *fontName) {
            std::shared_ptr<Font::Font> font = Render::OpenGLFontLoader::load(fontName, true);

            return Im3DFont {
                font->mTexture->mTextureHandle,
                font->mTextureSize,
                font->mGlyphs.data()
            };
        };
    }

    OpenGLImManager::~OpenGLImManager()
    {
        ImGui_ImplOpenGL3_Shutdown();
    }

    void OpenGLImManager::newFrame(float timeSinceLastFrame)
    {
        ImGuiIO &io = ImGui::GetIO();

        io.DeltaTime = timeSinceLastFrame;
        
        Vector2i size = mWindow.getScreenSpace().mSize;

        io.BackendPlatformUserData = &mWindow;

        io.DisplaySize = ImVec2(size.x / io.DisplayFramebufferScale.x, size.y / io.DisplayFramebufferScale.y);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        Im3D::NewFrame();
    }

}
}

UNIQUECOMPONENT(Engine::Tools::OpenGLImManager)

METATABLE_BEGIN(Engine::Tools::OpenGLImManager)
METATABLE_END(Engine::Tools::OpenGLImManager)

RegisterType(Engine::Tools::OpenGLImManager)