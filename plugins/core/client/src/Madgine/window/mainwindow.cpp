#include "../clientlib.h"

#include "mainwindow.h"

#include "../input/inputhandler.h"

#include "Interfaces/window/windowapi.h"

#include "../render/rendercontext.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"

#include "toolwindow.h"

#include "Interfaces/exception.h"

#include "Modules/generic/reverseIt.h"

#include "../render/rendertarget.h"

METATABLE_BEGIN(Engine::Window::MainWindow)
READONLY_PROPERTY(Components, components)
METATABLE_END(Engine::Window::MainWindow)

RegisterType(Engine::Window::MainWindow);

SERIALIZETABLE_BEGIN(Engine::Window::MainWindow)
FIELD(mComponents)
SERIALIZETABLE_END(Engine::Window::MainWindow)

namespace Engine {
namespace Window {

    MainWindow::MainWindow(const WindowSettings &settings, Input::InputHandler *input)
        : mComponents(*this)
        , mSettings(settings)
        , mExternalInput(input)
    {

        mLoop.addSetupSteps(
            [this]() {
                if (!callInit())
                    throw exception("Window Init Failed!");
                sync();
            },
            [this]() {
                unsync();
                callFinalize();
            });
    }

    MainWindow::~MainWindow()
    {
    }

    Rect2i MainWindow::getScreenSpace()
    {
        if (!mWindow)
            return { { 0, 0 }, { 0, 0 } };
        return { { mWindow->renderX(), mWindow->renderY() }, { mWindow->renderWidth(), mWindow->renderHeight() } };
    }

    const MadgineObject *MainWindow::parent() const
    {
        return nullptr;
    }

    bool MainWindow::init()
    {

        mWindow = sCreateWindow(mSettings);

        mWindow->addListener(this);

        if (!mExternalInput) {
            mInputHandlerSelector.emplace(*this, mWindow, this, 0);
        }

        mRenderContext.emplace(&mLoop);
        mRenderWindow = (*mRenderContext)->createRenderWindow(mWindow);
        addFrameListener(this);

        for (MainWindowComponentBase *comp : components()) {
            bool result = comp->callInit();
            assert(result);
        }

        return true;
    }

    void MainWindow::finalize()
    {
        for (MainWindowComponentBase *comp : components()) {
            comp->callFinalize();
        }

        removeFrameListener(this);

        mRenderWindow.reset();
        mRenderContext.reset();

        mInputHandlerSelector.reset();

        if (mWindow) {
            mWindow->removeListener(this);
            mWindow->destroy();
        }
    }

    ToolWindow *MainWindow::createToolWindow(const WindowSettings &settings)
    {
        return mToolWindows.emplace_back(std::make_unique<ToolWindow>(*this, settings)).get();
    }

    void MainWindow::destroyToolWindow(ToolWindow *w)
    {
        auto it = std::find_if(mToolWindows.begin(), mToolWindows.end(), [=](const std::unique_ptr<ToolWindow> &ptr) { return ptr.get() == w; });
        assert(it != mToolWindows.end());
        mToolWindows.erase(it);
    }

    Input::InputHandler *MainWindow::input()
    {
        return mExternalInput ? mExternalInput : *mInputHandlerSelector;
    }

    bool MainWindow::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        for (MainWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectKeyPress(arg))
                return true;
        }
        return false;
    }

    bool MainWindow::injectKeyRelease(const Input::KeyEventArgs &arg)
    {
        for (MainWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectKeyRelease(arg))
                return true;
        }
        return false;
    }

    bool MainWindow::injectPointerPress(const Input::PointerEventArgs &arg)
    {

        for (MainWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectPointerPress(arg))
                return true;
        }

        return false;
    }

    bool MainWindow::injectPointerRelease(const Input::PointerEventArgs &arg)
    {

        for (MainWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectPointerRelease(arg))
                return true;
        }

        return false;
    }

    bool MainWindow::injectPointerMove(const Input::PointerEventArgs &arg)
    {

        for (MainWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectPointerMove(arg))
                return true;
        }

        return false;
    }

    Window *MainWindow::window() const
    {
        return mWindow;
    }

    Render::RenderContext *MainWindow::getRenderer()
    {
        return mRenderContext->get();
    }

    void MainWindow::onClose()
    {
        mWindow = nullptr;
        callFinalize();
    }

    void MainWindow::onRepaint()
    {
        //update();
    }

    void MainWindow::onResize(size_t width, size_t height)
    {
        mRenderWindow->resize({ static_cast<int>(width), static_cast<int>(height) });
        input()->onResize(width, height);
        applyClientSpaceResize();
    }

    void MainWindow::applyClientSpaceResize(MainWindowComponentBase *component)
    {
        if (!mWindow)
            return;

        Rect2i space;
        if (!component)
            space = { { 0, 0 }, { mWindow->renderWidth(), mWindow->renderHeight() } };
        else
            space = component->getChildClientSpace();

        for (MainWindowComponentBase *comp : reverseIt(components())) {
            if (component) {
                if (component == comp) {
                    component = nullptr;
                }
            } else {
                comp->onResize(space);
                space = comp->getChildClientSpace();
            }
        }
    }

    bool MainWindow::frameStarted(std::chrono::microseconds)
    {
        return true;
    }

    bool MainWindow::frameRenderingQueued(std::chrono::microseconds, Threading::ContextMask)
    {

        return mWindow;
    }

    bool MainWindow::frameEnded(std::chrono::microseconds)
    {
        (*mRenderContext)->render();
        sUpdate();
        return mWindow;
    }

    MainWindowComponentBase &MainWindow::getWindowComponent(size_t i, bool init)
    {
        MainWindowComponentBase &component = mComponents.get(i);
        if (init) {
            component.callInit();
        }
        return component.getSelf(init);
    }

    void MainWindow::addFrameListener(Threading::FrameListener *listener)
    {
        mLoop.addFrameListener(listener);
    }

    void MainWindow::removeFrameListener(Threading::FrameListener *listener)
    {
        mLoop.removeFrameListener(listener);
    }

    void MainWindow::singleFrame()
    {
        mLoop.singleFrame();
    }

    Threading::FrameLoop &MainWindow::frameLoop()
    {
        return mLoop;
    }

    void MainWindow::shutdown()
    {
        mLoop.shutdown();
    }

    Render::RenderTarget *MainWindow::getRenderWindow()
    {
        return mRenderWindow.get();
    }

}
}
