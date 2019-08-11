#include "../../clientlib.h"

#include "toolwindow.h"

#include "toplevelwindow.h"

#include "../../input/inputhandler.h"

#include "Interfaces/window/windowapi.h"

#include "../../render/renderwindow.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "../guisystem.h"

#include "Madgine/app/application.h"

namespace Engine {

namespace GUI {
    ToolWindow::ToolWindow(TopLevelWindow &parent, const Window::WindowSettings &settings)
        : mParent(parent)
    {
        mWindow = Window::sCreateWindow(settings);

        mWindow->addListener(this);

        mInputHandlerSelector.emplace(mWindow, parent.gui().app(false), &parent);

        mRenderWindow = parent.gui().renderer().createWindow(mWindow, nullptr, parent.getRenderer());
    }

    ToolWindow::~ToolWindow()
    {
        mParent.gui().app(false).removeFrameListener(input());
        mWindow->removeListener(this);

        mWindow->destroy();
    }

    void ToolWindow::close()
    {
        //mGui.closeTopLevelWindow(this);
    }

    /*void TopLevelWindow::showCursor()
		{
			setCursorVisibility(true);
		}

		void TopLevelWindow::hideCursor()
		{
			setCursorVisibility(false);
		}*/

    Vector3 ToolWindow::getScreenSize()
    {
        return { (float)mWindow->renderWidth(), (float)mWindow->renderHeight(), 1.0f };
    }

    Input::InputHandler *ToolWindow::input()
    {
        return *mInputHandlerSelector;
    }

    Window::Window *ToolWindow::window()
    {
        return mWindow;
    }

    Render::RenderWindow *ToolWindow::getRenderer()
    {
        return mRenderWindow.get();
    }

    void ToolWindow::beginFrame()
    {
        mRenderWindow->makeCurrent();
        mWindow->beginFrame();
    }

    void ToolWindow::endFrame()
    {
        mWindow->endFrame();
        mParent.getRenderer()->makeCurrent();
    }

    void ToolWindow::onClose()
    {
        close();
    }

    void ToolWindow::onRepaint()
    {
        //update();
    }

    void ToolWindow::onResize(size_t width, size_t height)
    {
        input()->onResize(width, height);
   }

 
}
}

METATABLE_BEGIN(Engine::GUI::ToolWindow)
METATABLE_END(Engine::GUI::ToolWindow)

RegisterType(Engine::GUI::ToolWindow);