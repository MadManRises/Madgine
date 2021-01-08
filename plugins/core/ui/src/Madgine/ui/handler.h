#pragma once

#include "Modules/keyvalueutil/virtualscope.h"
#include "Modules/madgineobject/madgineobject.h"
#include "Modules/serialize/virtualserializableunit.h"
#include "Modules/threading/slot.h"

#include "Modules/uniquecomponent/uniquecomponent.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

namespace Engine {
namespace UI {
    struct WindowDescriptor {
        std::string mWidgetName;
        std::function<bool(Widgets::WidgetBase *)> mInit;
    };

    struct MADGINE_UI_EXPORT Handler : MadgineObject<Handler>, Serialize::VirtualSerializableUnitBase<VirtualScopeBase, Serialize::SerializableUnitBase> {
        SERIALIZABLEUNIT(Handler);

        Handler(UIManager &ui);
        virtual ~Handler() = default;

        virtual void onMouseVisibilityChanged(bool b);

        Widgets::WidgetBase *widget() const;
        virtual void setWidget(Widgets::WidgetBase *w);

        virtual void sizeChanged();

        virtual std::string_view key() const = 0;

        //virtual App::Application &app(bool = true) override;
        const UIManager *parent() const;
        //UIManager &ui(bool = true);

        template <typename T>
        T &getGuiHandler(bool init = true)
        {
            return static_cast<T &>(getGuiHandler(component_index<T>(), init));
        }

        GuiHandlerBase &getGuiHandler(size_t i, bool = true);

        template <typename T>
        T &getGameHandler(bool init = true)
        {
            return static_cast<T &>(getGameHandler(component_index<T>(), init));
        }

        GameHandlerBase &getGameHandler(size_t i, bool = true);

        void registerWidget(const std::string &name, std::function<bool(Widgets::WidgetBase *)> init);

    protected:
        virtual bool init();
        virtual void finalize();

        friend struct MadgineObject<Handler>;

        virtual void onPointerMove(const Input::PointerEventArgs &me);

        virtual void onPointerDown(const Input::PointerEventArgs &me);

        virtual void onPointerUp(const Input::PointerEventArgs &me);

        virtual bool onKeyPress(const Input::KeyEventArgs &evt);

    public:
        void injectPointerMove(const Input::PointerEventArgs &evt);
        void injectPointerDown(const Input::PointerEventArgs &evt);
        void injectPointerUp(const Input::PointerEventArgs &evt);
        bool injectKeyPress(const Input::KeyEventArgs &evt);

    protected:
        Widgets::WidgetBase *mWidget = nullptr;

        UIManager &mUI;

    private:
        std::list<WindowDescriptor> mWidgets;

        Threading::Slot<&Handler::injectPointerMove> mPointerMoveSlot;
        Threading::Slot<&Handler::injectPointerDown> mPointerDownSlot;
        Threading::Slot<&Handler::injectPointerUp> mPointerUpSlot;
    };
}
}
