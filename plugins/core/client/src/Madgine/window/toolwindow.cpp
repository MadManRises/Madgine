#include "../clientlib.h"

#include "toolwindow.h"

#include "mainwindow.h"

#include "Interfaces/window/windowapi.h"

#include "../render/rendercontext.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "../render/rendertarget.h"

namespace Engine {

namespace Window {
    ToolWindow::ToolWindow(MainWindow &parent, const WindowSettings &settings)
        : mParent(parent)
    {
        mOsWindow = sCreateWindow(settings, this);

        mRenderWindow = parent.getRenderer()->createRenderWindow(mOsWindow);
    }

    ToolWindow::~ToolWindow()
    {
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
        InterfacesVector size = mOsWindow->renderSize();
        return Vector3{ Vector2 { static_cast<float>(size.x), static_cast<float>(size.y) }, 1.0f };
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
        mRenderWindow->resize({ size.x, size.y });
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
