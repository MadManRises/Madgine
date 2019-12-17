#include "../clientlib.h"

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

    void TopLevelWindowComponentBase::onResize(const Rect2i &space)
    {
        mClientSpace = space;
    }

    Rect2i TopLevelWindowComponentBase::getScreenSpace() const
    {
        return mWindow.getScreenSpace();
    }

    const Rect2i &TopLevelWindowComponentBase::getClientSpace() const
    {
        return mClientSpace;
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