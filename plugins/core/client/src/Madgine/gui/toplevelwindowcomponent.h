#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalue/scopebase.h"

#include "Modules/serialize/serializableunit.h"

#include "Modules/math/rect2i.h"

#include "../input/inputlistener.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace GUI {

    struct MADGINE_CLIENT_EXPORT TopLevelWindowComponentBase : ScopeBase, MadgineObject, Serialize::SerializableUnitBase, Input::InputListener {
        TopLevelWindowComponentBase(TopLevelWindow &window, int priority);
        virtual ~TopLevelWindowComponentBase() = default;

        TopLevelWindow &window() const;

        virtual const MadgineObject *parent() const override;

        TopLevelWindowComponentBase &getSelf(bool = true);

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

        TopLevelWindowComponentBase &getWindowComponent(size_t i, bool = true);

        const int mPriority;

    protected:
        TopLevelWindow &mWindow;
        Rect2i mClientSpace;
    };

}
}

