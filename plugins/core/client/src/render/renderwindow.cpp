#include "../clientlib.h"
#include "renderwindow.h"
#include "../gui/guisystem.h"
#include "../gui/widgets/toplevelwindow.h"
#include "Interfaces/window/windowapi.h"
#include "Madgine/app/application.h"
#include "rendertarget.h"

#include "Modules/debug/profiler/profiler.h"

namespace Engine {
namespace Render {

    RenderWindow::RenderWindow(Window::Window *w)
        : mWindow(w)
    {
    }

    RenderWindow::~RenderWindow()
    {
    }

    void RenderWindow::beginFrame()
    {
        mWindow->beginFrame();
    }

    void RenderWindow::endFrame()
    {
        mWindow->endFrame();
    }

    void RenderWindow::addRenderTarget(RenderTarget *target)
    {
        mRenderTargets.push_back(target);
    }

    void RenderWindow::removeRenderTarget(RenderTarget *target)
    {
        mRenderTargets.erase(std::find(mRenderTargets.begin(), mRenderTargets.end(), target));
    }

    void RenderWindow::updateRenderTargets()
    {
        for (RenderTarget *target : mRenderTargets)
            target->render();
    }

    Window::Window *RenderWindow::window()
    {
        return mWindow;
    }

}
}