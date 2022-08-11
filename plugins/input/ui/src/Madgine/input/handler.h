#pragma once

#include "Meta/keyvalue/virtualscope.h"
#include "Meta/serialize/hierarchy/virtualserializableunit.h"
#include "Modules/threading/connectionstore.h"
#include "Modules/threading/madgineobject.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Input {

    struct MADGINE_UI_EXPORT Handler : VirtualScopeBase<>, Threading::MadgineObject<Handler> {
        SERIALIZABLEUNIT(Handler)

        Handler(UIManager &ui, std::string_view widgetName);
        virtual ~Handler() = default;

        virtual void onMouseVisibilityChanged(bool b);

        void onUpdate();
        Widgets::WidgetBase *widget() const;
        virtual void setWidget(Widgets::WidgetBase *w);

        virtual void sizeChanged();

        virtual std::string_view key() const = 0;

        template <typename T>
        T &getGuiHandler()
        {
            return static_cast<T &>(getGuiHandler(UniqueComponent::component_index<T>()));
        }

        GuiHandlerBase &getGuiHandler(size_t i);

        template <typename T>
        T &getGameHandler()
        {
            return static_cast<T &>(getGameHandler(UniqueComponent::component_index<T>()));
        }

        GameHandlerBase &getGameHandler(size_t i);

        Threading::TaskQueue *viewTaskQueue() const;
        Threading::TaskQueue *modelTaskQueue() const;

    protected:
        virtual Threading::Task<bool> init();
        virtual Threading::Task<void> finalize();

        friend struct MadgineObject<Handler>;

        virtual void onPointerMove(const PointerEventArgs &me);
        virtual void onPointerClick(const PointerEventArgs &me);

        virtual void onDragBegin(const PointerEventArgs &me);
        virtual void onDragMove(const PointerEventArgs &me);
        virtual void onDragEnd(const PointerEventArgs &me);

        virtual bool onKeyPress(const KeyEventArgs &evt);

        virtual void onAxisEvent(const AxisEventArgs &evt);

    public:
        void injectPointerMove(const PointerEventArgs &evt);
        void injectPointerClick(const PointerEventArgs &evt);
        void injectDragBegin(const PointerEventArgs &evt);
        void injectDragMove(const PointerEventArgs &evt);
        void injectDragEnd(const PointerEventArgs &evt);
        bool injectKeyPress(const KeyEventArgs &evt);
        void injectAxisEvent(const AxisEventArgs &evt);

    protected:
        std::string_view mWidgetName;
        Widgets::WidgetBase *mWidget = nullptr;

        UIManager &mUI;

        Engine::Threading::ConnectionStore mConStore;
    };
}
}
