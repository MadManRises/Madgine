#include "../clientlib.h"

#include "toolwindow.h"

#include "mainwindow.h"

#include "../input/inputhandler.h"

#include "Interfaces/window/windowapi.h"

#include "../render/rendercontext.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "../render/rendertarget.h"

namespace Engine {

namespace Window {
    ToolWindow::ToolWindow(MainWindow &parent, const WindowSettings &settings)
    {
        mOsWindow = sCreateWindow(settings);

        mOsWindow->addListener(this);

        mInputHandlerSelector.emplace(parent, mOsWindow, &parent, 0);

        mRenderWindow = parent.getRenderer()->createRenderWindow(mOsWindow);
    }

    ToolWindow::~ToolWindow()
    {
        mOsWindow->removeListener(this);

        mOsWindow->destroy();
    }

    void ToolWindow::close()
    {
        //mGui.closeMainWindow(this);
    }

    /*void MainWindow::showCursor()
		{
			setCursorVisibility(true);
		}

		void MainWindow::hideCursor()
		{
			setCursorVisibility(false);
		}*/

    Vector3 ToolWindow::getScreenSize()
    {
        return { (float)mOsWindow->renderWidth(), (float)mOsWindow->renderHeight(), 1.0f };
    }

    Input::InputHandler *ToolWindow::input()
    {
        return *mInputHandlerSelector;
    }

    OSWindow *ToolWindow::osWindow()
    {
        return mOsWindow;
    }

    Render::RenderTarget *ToolWindow::getRenderer()
    {
        return mRenderWindow.get();
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
        mRenderWindow->resize({ static_cast<int>(width), static_cast<int>(height) });
        input()->onResize(width, height);
   }

 
}
}

METATABLE_BEGIN(Engine::Window::ToolWindow)
METATABLE_END(Engine::Window::ToolWindow)

RegisterType(Engine::Window::ToolWindow);