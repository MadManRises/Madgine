#pragma once

#include "Modules/keyvalue/scopebase.h"
#include "Modules/madgineobject/madgineobject.h"
#include "Modules/threading/slot.h"
#include "Modules/serialize/serializableunit.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace UI {
    struct MADGINE_CLIENT_EXPORT WindowDescriber {
        WindowDescriber(const std::string &widgetName,
            std::function<bool(GUI::WidgetBase *)> init)
            : mWidgetName(widgetName)
            , mInit(init)
        {
        }

        std::string mWidgetName;
        std::function<bool(GUI::WidgetBase *)> mInit;
    };

    class MADGINE_CLIENT_EXPORT Handler : public MadgineObject, public ScopeBase, public Serialize::SerializableUnit<Handler> {
        SERIALIZABLEUNIT;
    public:
        Handler(UIManager &ui);
        virtual ~Handler() = default;

        virtual void onMouseVisibilityChanged(bool b);

        GUI::WidgetBase *widget() const;
        virtual void setWidget(GUI::WidgetBase *w);

        virtual void sizeChanged();

        virtual const char *key() const = 0;

        //virtual App::Application &app(bool = true) override;
        virtual const MadgineObject *parent() const override;
        UIManager &ui(bool = true);

        template <class T>
        T &getGuiHandler(bool init = true)
        {
            return static_cast<T &>(getGuiHandler(component_index<T>(), init));
        }

        GuiHandlerBase &getGuiHandler(size_t i, bool = true);

        template <class T>
        T &getGameHandler(bool init = true)
        {
            return static_cast<T &>(getGameHandler(component_index<T>(), init));
        }

        GameHandlerBase &getGameHandler(size_t i, bool = true);

        void registerWidget(const std::string &name, std::function<bool(GUI::WidgetBase *)> init);

    protected:
        bool init() override;
        void finalize() override;

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
        GUI::WidgetBase *mWidget = nullptr;

        UIManager &mUI;

    private:
        std::list<WindowDescriber> mWidgets;

        Threading::Slot<&Handler::injectPointerMove> mPointerMoveSlot;
        Threading::Slot<&Handler::injectPointerDown> mPointerDownSlot;
        Threading::Slot<&Handler::injectPointerUp> mPointerUpSlot;
    };
}
}
