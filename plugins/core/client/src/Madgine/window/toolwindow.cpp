#include "../clientlib.h"

#include "toolwindow.h"

#include "mainwindow.h"

#include "Interfaces/window/windowapi.h"

#include "../render/rendercontext.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "../render/rendertarget.h"

namespace Engine {

namespace Window {
    ToolWindow::ToolWindow(MainWindow &parent, const WindowSettings &settings)
        : mParent(parent)
    {
        mOsWindow = sCreateWindow(settings);

        mOsWindow->addListener(this);

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
        return Vector3{ Vector2 { mOsWindow->renderSize() }, 1.0f };
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

    void ToolWindow::onResize(const InterfacesVector &size)
    {
        mRenderWindow->resize(size);
    }

    bool ToolWindow::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        return mParent.injectKeyPress(arg);
    }

    bool ToolWindow::injectKeyRelease(const Input::KeyEventArgs &arg)
    {
        return mParent.injectKeyRelease(arg);
    }

    bool ToolWindow::injectPointerPress(const Input::PointerEventArgs &arg)
    {
        return mParent.injectPointerPress(arg);
    }

    bool ToolWindow::injectPointerRelease(const Input::PointerEventArgs &arg)
    {
        return mParent.injectPointerRelease(arg);
    }

    bool ToolWindow::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        return mParent.injectPointerMove(arg);
    }

}
}

METATABLE_BEGIN(Engine::Window::ToolWindow)
METATABLE_END(Engine::Window::ToolWindow)

RegisterType(Engine::Window::ToolWindow);