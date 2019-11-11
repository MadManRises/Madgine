#include "../../clientlib.h"

#include "toplevelwindowcomponent.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"

#include "toplevelwindow.h"

RegisterType(Engine::GUI::TopLevelWindowComponentBase);

DEFINE_UNIQUE_COMPONENT(Engine::GUI, TopLevelWindow)

namespace Engine {
namespace GUI {

    TopLevelWindowComponentBase::TopLevelWindowComponentBase(TopLevelWindow &window, int priority)
        : mPriority(priority)
        , mWindow(window)
    {
    }

    TopLevelWindow &TopLevelWindowComponentBase::window() const
    {
        return mWindow;
    }

    const MadgineObject *TopLevelWindowComponentBase::parent() const
    {
        return &mWindow;
    }

    TopLevelWindowComponentBase &TopLevelWindowComponentBase::getSelf(bool init)
    {
        if (init) {
            checkDependency();
        }
        return *this;
    }

    void TopLevelWindowComponentBase::calculateAvailableScreenSpace(Window::Window *w, Vector3i &pos, Vector3i &size)
    {
    }

    void TopLevelWindowComponentBase::onResize(size_t width, size_t height)
    {
    }

    Rect2i TopLevelWindowComponentBase::getScreenSpace() const
    {
        return mWindow.getScreenSpace();
    }

    TopLevelWindowComponentBase &TopLevelWindowComponentBase::getWindowComponent(size_t i, bool init)
    {
        if (init) {
            checkInitState();
        }
        return mWindow.getWindowComponent(i, init);
    }

    Rect2i TopLevelWindowComponentBase::getChildClientSpace()
    {
        return mClientSpace;
    }

}
}