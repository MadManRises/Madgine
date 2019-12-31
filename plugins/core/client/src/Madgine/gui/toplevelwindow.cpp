#include "../clientlib.h"

#include "toplevelwindow.h"

#include "../input/inputhandler.h"

#include "Interfaces/window/windowapi.h"

#include "../render/rendercontext.h"

#include "Modules/generic/transformIt.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"

#include "Modules/debug/profiler/profiler.h"

#include "toolwindow.h"

#include "Interfaces/exception.h"

#include "Modules/generic/reverseIt.h"

#include "../render/rendertarget.h"

METATABLE_BEGIN(Engine::GUI::TopLevelWindow)
READONLY_PROPERTY(Components, components)
METATABLE_END(Engine::GUI::TopLevelWindow)

RegisterType(Engine::GUI::TopLevelWindow);

SERIALIZETABLE_BEGIN(Engine::GUI::TopLevelWindow)
FIELD(mComponents)
SERIALIZETABLE_END(Engine::GUI::TopLevelWindow)

namespace Engine {

namespace GUI {

    TopLevelWindow::TopLevelWindow(const Window::WindowSettings &settings)
        : mComponents(*this)
        , mSettings(settings)
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

    TopLevelWindow::~TopLevelWindow()
    {
    }

    Rect2i TopLevelWindow::getScreenSpace()
    {
        if (!mWindow)
            return { { 0, 0 }, { 0, 0 } };
        return { { mWindow->renderX(), mWindow->renderY() }, { mWindow->renderWidth(), mWindow->renderHeight() } };
    }

    const MadgineObject *TopLevelWindow::parent() const
    {
        return nullptr;
    }

    bool TopLevelWindow::init()
    {

        mWindow = Window::sCreateWindow(mSettings);

        mWindow->addListener(this);

        if (mSettings.mInput) {
            mExternalInput = static_cast<Input::InputHandler *>(mSettings.mInput);
        } else {
            mInputHandlerSelector.emplace(*this, mWindow, this, 0);
        }

        mRenderContext.emplace();
        mRenderWindow = (*mRenderContext)->createRenderWindow(mWindow);
        addFrameListener(this);

        for (TopLevelWindowComponentBase *comp : uniquePtrToPtr(mComponents)) {
            bool result = comp->callInit();
            assert(result);
        }

        return true;
    }

    void TopLevelWindow::finalize()
    {
        for (TopLevelWindowComponentBase *comp : uniquePtrToPtr(mComponents)) {
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

    ToolWindow *TopLevelWindow::createToolWindow(const Window::WindowSettings &settings)
    {
        return mToolWindows.emplace_back(std::make_unique<ToolWindow>(*this, settings)).get();
    }

    void TopLevelWindow::destroyToolWindow(ToolWindow *w)
    {
        auto it = std::find_if(mToolWindows.begin(), mToolWindows.end(), [=](const std::unique_ptr<ToolWindow> &ptr) { return ptr.get() == w; });
        assert(it != mToolWindows.end());
        mToolWindows.erase(it);
    }

    Input::InputHandler *TopLevelWindow::input()
    {
        return mExternalInput ? mExternalInput : *mInputHandlerSelector;
    }

    bool TopLevelWindow::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectKeyPress(arg))
                return true;
        }
        return false;
    }

    bool TopLevelWindow::injectKeyRelease(const Input::KeyEventArgs &arg)
    {
        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectKeyRelease(arg))
                return true;
        }
        return false;
    }

    bool TopLevelWindow::injectPointerPress(const Input::PointerEventArgs &arg)
    {

        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectPointerPress(arg))
                return true;
        }

        return false;
    }

    bool TopLevelWindow::injectPointerRelease(const Input::PointerEventArgs &arg)
    {

        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectPointerRelease(arg))
                return true;
        }

        return false;
    }

    bool TopLevelWindow::injectPointerMove(const Input::PointerEventArgs &arg)
    {

        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectPointerMove(arg))
                return true;
        }

        return false;
    }

    Window::Window *TopLevelWindow::window() const
    {
        return mWindow;
    }

    Render::RenderContext *TopLevelWindow::getRenderer()
    {
        return mRenderContext->get();
    }

    void TopLevelWindow::onClose()
    {
        mWindow = nullptr;
        callFinalize();
    }

    void TopLevelWindow::onRepaint()
    {
        //update();
    }

    void TopLevelWindow::onResize(size_t width, size_t height)
    {
        input()->onResize(width, height);
        applyClientSpaceResize();
    }

    void TopLevelWindow::applyClientSpaceResize(TopLevelWindowComponentBase *component)
    {
        if (!mWindow)
            return;

        Rect2i space;
        if (!component)
            space = { { 0, 0 }, { mWindow->renderWidth(), mWindow->renderHeight() } };
        else
            space = component->getChildClientSpace();

        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
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

    bool TopLevelWindow::frameStarted(std::chrono::microseconds)
    {
        return true;
    }

    bool TopLevelWindow::frameRenderingQueued(std::chrono::microseconds, Threading::ContextMask)
    {

        return mWindow;
    }

    bool TopLevelWindow::frameEnded(std::chrono::microseconds)
    {
        (*mRenderContext)->render();
        Window::sUpdate();
        return mWindow;
    }

    TopLevelWindowComponentBase &TopLevelWindow::getWindowComponent(size_t i, bool init)
    {
        TopLevelWindowComponentBase &component = mComponents.get(i);
        if (init) {
            component.callInit();
        }
        return component.getSelf(init);
    }

    void TopLevelWindow::addFrameListener(Threading::FrameListener *listener)
    {
        mLoop.addFrameListener(listener);
    }

    void TopLevelWindow::removeFrameListener(Threading::FrameListener *listener)
    {
        mLoop.removeFrameListener(listener);
    }

    void TopLevelWindow::singleFrame()
    {
        mLoop.singleFrame();
    }

    Threading::FrameLoop &TopLevelWindow::frameLoop()
    {
        return mLoop;
    }

    void TopLevelWindow::shutdown()
    {
        mLoop.shutdown();
    }

    Render::RenderTarget *TopLevelWindow::getRenderWindow()
    {
        return mRenderWindow.get();
    }

}
}
