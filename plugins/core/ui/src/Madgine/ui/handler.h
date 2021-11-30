#pragma once

#include "Meta/keyvalue/virtualscope.h"
#include "Meta/serialize/virtualserializableunit.h"
#include "Modules/threading/connectionstore.h"
#include "madgineobject/madgineobject.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace UI {

    struct MADGINE_UI_EXPORT Handler : MadgineObject<Handler>, Serialize::VirtualSerializableDataBase<VirtualScopeBase<>, Serialize::SerializableDataUnit> {
        SERIALIZABLEUNIT(Handler);

        Handler(UIManager &ui);
        virtual ~Handler() = default;

        virtual void onMouseVisibilityChanged(bool b);

        Widgets::WidgetBase *widget() const;
        virtual void setWidget(Widgets::WidgetBase *w);

        virtual void sizeChanged();

        virtual std::string_view key() const = 0;

        template <typename T>
        T &getGuiHandler()
        {
            return static_cast<T &>(getGuiHandler(component_index<T>()));
        }

        GuiHandlerBase &getGuiHandler(size_t i);

        template <typename T>
        T &getGameHandler()
        {
            return static_cast<T &>(getGameHandler(component_index<T>()));
        }

        GameHandlerBase &getGameHandler(size_t i);

        Threading::TaskQueue *taskQueue() const;

    protected:
        virtual bool init();
        virtual void finalize();

        friend struct MadgineObject<Handler>;

        virtual void onPointerMove(const Input::PointerEventArgs &me);

        virtual void onPointerDown(const Input::PointerEventArgs &me);

        virtual void onPointerUp(const Input::PointerEventArgs &me);

        virtual bool onKeyPress(const Input::KeyEventArgs &evt);

        virtual void onAxisEvent(const Input::AxisEventArgs &evt);

    public:
        void injectPointerMove(const Input::PointerEventArgs &evt);
        void injectPointerDown(const Input::PointerEventArgs &evt);
        void injectPointerUp(const Input::PointerEventArgs &evt);
        bool injectKeyPress(const Input::KeyEventArgs &evt);
        void injectAxisEvent(const Input::AxisEventArgs &evt);

    protected:
        Widgets::WidgetBase *mWidget = nullptr;

        UIManager &mUI;

        Engine::Threading::ConnectionStore mConStore;
    };
}
}
