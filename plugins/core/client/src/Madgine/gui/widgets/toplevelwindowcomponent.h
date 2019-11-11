#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalue/scopebase.h"

#include "Modules/serialize/serializableunit.h"

#include "Modules/uniquecomponent/uniquecomponentdefine.h"

#include "Modules/math/rect2i.h"

#include "../../input/inputlistener.h"

namespace Engine {
namespace GUI {

    struct MADGINE_CLIENT_EXPORT TopLevelWindowComponentBase : ScopeBase, MadgineObject, Serialize::SerializableUnitBase, Input::InputListener {
        TopLevelWindowComponentBase(TopLevelWindow &window, int priority);
        virtual ~TopLevelWindowComponentBase() = default;

        TopLevelWindow &window() const;

        virtual const MadgineObject *parent() const override;

        TopLevelWindowComponentBase &getSelf(bool = true);

        virtual const char *key() const = 0;

        virtual void calculateAvailableScreenSpace(Window::Window *w, Vector3i &pos, Vector3i &size);
        virtual void onResize(size_t width, size_t height);

        Rect2i getScreenSpace() const;        

        template <class T>
        T &getWindowComponent(bool init = true)
        {
            return static_cast<T &>(getWindowComponent(T::component_index(), init));
        }

        TopLevelWindowComponentBase &getWindowComponent(size_t i, bool = true);

        const int mPriority;

		virtual Rect2i getChildClientSpace();

		Rect2i mClientSpace;

    protected:
        TopLevelWindow &mWindow;		
    };

    struct TopLevelWindowComponentComparator {
        using cmp_type = int;

        bool operator()(const std::unique_ptr<TopLevelWindowComponentBase> &first, const std::unique_ptr<TopLevelWindowComponentBase> &second) const
        {
            return first->mPriority < second->mPriority;
        }
    };

}
}

DECLARE_UNIQUE_COMPONENT(Engine::GUI, TopLevelWindow, TopLevelWindowComponentBase, TopLevelWindow &);