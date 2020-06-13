#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalue/virtualscopebase.h"

#include "Modules/serialize/serializableunit.h"

#include "Modules/math/rect2i.h"

#include "../input/inputlistener.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Window {

    struct MADGINE_CLIENT_EXPORT MainWindowComponentBase : VirtualScopeBase, MadgineObject, Serialize::SerializableUnitBase, Input::InputListener {
        MainWindowComponentBase(MainWindow &window, int priority);
        virtual ~MainWindowComponentBase() = default;

        MainWindow &window() const;

        virtual const MadgineObject *parent() const override;

        MainWindowComponentBase &getSelf(bool = true);

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

        const int mPriority;

    protected:
        MainWindow &mWindow;
        Rect2i mClientSpace;
    };

}
}

RegisterType(Engine::Window::MainWindowComponentBase);