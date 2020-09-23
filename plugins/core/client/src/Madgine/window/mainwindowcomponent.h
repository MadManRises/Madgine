#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalueutil/virtualscopebase.h"

#include "Modules/serialize/serializableunit.h"

#include "Modules/math/rect2i.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Window {

    struct MADGINE_CLIENT_EXPORT MainWindowComponentBase : VirtualScopeBase<>, Serialize::SerializableUnitBase, MadgineObject<MainWindowComponentBase> {
        MainWindowComponentBase(MainWindow &window, int priority);
        virtual ~MainWindowComponentBase() = default;

        MainWindow &window() const;

        const MainWindow *parent() const;

        virtual bool init() = 0;
        virtual void finalize() = 0;

        virtual std::string_view key() const = 0;

        virtual void onResize(const Rect2i &space);

        Rect2i getScreenSpace() const;
        const Rect2i &getClientSpace() const;
        virtual Rect2i getChildClientSpace();

        template <typename T>
        T &getWindowComponent(bool init = true)
        {
            return static_cast<T &>(getWindowComponent(component_index<T>(), init));
        }

        MainWindowComponentBase &getWindowComponent(size_t i, bool = true);

        virtual bool injectKeyPress(const Input::KeyEventArgs &arg) { return false; };
        virtual bool injectKeyRelease(const Input::KeyEventArgs &arg) { return false; }
        virtual bool injectPointerPress(const Input::PointerEventArgs &arg) { return false; }
        virtual bool injectPointerRelease(const Input::PointerEventArgs &arg) { return false; }
        virtual bool injectPointerMove(const Input::PointerEventArgs &arg) { return false; }

        const int mPriority;

    protected:
        MainWindow &mWindow;
        Rect2i mClientSpace;
    };

}
}

RegisterType(Engine::Window::MainWindowComponentBase);