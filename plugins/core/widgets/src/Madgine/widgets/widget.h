#pragma once

#include "Meta/math/matrix3.h"

#include "Modules/threading/signal.h"
#include "widgetclass.h"

#include "Generic/container/transformIt.h"

#include "Meta/serialize/hierarchy/virtualserializableunit.h"

#include "Meta/keyvalue/virtualscope.h"

#include "Madgine/render/texturedescriptor.h"
#include "util/texturesettings.h"
#include "util/vertex.h"

#include "condition.h"
#include "properties.h"

namespace Engine {
namespace Widgets {

    struct MADGINE_WIDGETS_EXPORT WidgetBase : VirtualScope<WidgetBase, Serialize::VirtualData<WidgetBase, Serialize::VirtualSerializableDataBase<VirtualScopeBase<>, Serialize::SerializableDataUnit>>> {
        SERIALIZABLEUNIT(WidgetBase)

        WidgetBase(WidgetManager &manager, WidgetBase *parent = nullptr);

        WidgetBase(const WidgetBase &) = delete;

        virtual ~WidgetBase();

        virtual WidgetClass getClass() const;

        WidgetManager &manager();
        const std::string &key() const;

        void destroy();

        void show();
        void hide();

        void setSize(const Matrix3 &size);
        const Matrix3 &getSize();
        void setPos(const Matrix3 &pos);
        const Matrix3 &getPos() const;

        Vector3 getAbsoluteSize() const;
        Vector2 getAbsolutePosition() const;

        void applyGeometry(const Vector3 &parentSize, const Vector2 &parentPos = Vector2::ZERO);
        Geometry calculateGeometry(uint16_t activeConditions, GeometrySourceInfo *source = nullptr);

        WidgetBase *createChild(WidgetClass _class);
        template <typename WidgetType = WidgetBase>
        MADGINE_WIDGETS_EXPORT WidgetType *createChild();

        WidgetBase *getChildRecursive(std::string_view name);
        template <typename T>
        T *getChildRecursive(std::string_view name)
        {
            return dynamic_cast<T *>(getChildRecursive(name));
        }

        decltype(auto) children() const
        {
            return uniquePtrToPtr(static_cast<const std::vector<std::unique_ptr<WidgetBase>> &>(mChildren));
        }

        void setParent(WidgetBase *parent);
        WidgetBase *getParent() const;

        bool isFocused() const;

        bool dragging() const;
        void abortDrag();

        virtual bool injectPointerClick(const Input::PointerEventArgs &arg);
        virtual bool injectPointerMove(const Input::PointerEventArgs &arg);
        virtual bool injectPointerEnter(const Input::PointerEventArgs &arg);
        virtual bool injectPointerLeave(const Input::PointerEventArgs &arg);
        virtual bool injectDragBegin(const Input::PointerEventArgs &arg);
        virtual bool injectDragMove(const Input::PointerEventArgs &arg);
        virtual bool injectDragEnd(const Input::PointerEventArgs &arg);
        virtual void injectDragAbort();
        virtual bool injectAxisEvent(const Input::AxisEventArgs &arg);
        virtual bool injectKeyPress(const Input::KeyEventArgs &arg);

        Threading::SignalStub<const Input::PointerEventArgs &> &pointerMoveEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerClickEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerEnterEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerLeaveEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &dragBeginEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &dragMoveEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &dragEndEvent();
        Threading::SignalStub<> &dragAbortEvent();
        Threading::SignalStub<const Input::AxisEventArgs &> &axisEvent();
        Threading::SignalStub<const Input::KeyEventArgs &> &keyEvent();

        bool containsPoint(const Vector2 &point, const Rect2i &screenSpace, float extend = 0.0f) const;

        virtual std::vector<std::pair<std::vector<Vertex>, TextureSettings>> vertices(const Vector3 &screenSize, size_t layer = 0);        

        uint16_t fetchActiveConditions(std::vector<Condition *> *conditions = nullptr);

        void addConditional(uint16_t mask);
        PropertyRange conditionals();

        void setPosValue(uint16_t index, float value, uint16_t mask = 0);

        void setSizeValue(uint16_t index, float value, uint16_t mask = 0);
        void unsetSizeValue(uint16_t index, uint16_t mask);

        bool mVisible = true;
        std::string mName = "Unnamed";

        std::vector<Condition> mConditions;

    protected:
        Serialize::StreamResult readWidget(Serialize::FormattedSerializeStream &in, std::unique_ptr<WidgetBase> &widget);
        const char *writeWidget(Serialize::FormattedSerializeStream &out, const std::unique_ptr<WidgetBase> &widget) const;

        virtual void sizeChanged(const Vector3i &pixelSize);

        size_t depth(size_t layer);

        uint16_t fetchActiveConditionsImpl(std::vector<Condition *> &conditions);

        bool evalCondition(Condition &cond);

    protected:
        void destroyChild(WidgetBase *w);

        Threading::Signal<const Input::PointerEventArgs &> mPointerMoveSignal, mPointerClickSignal, mPointerEnterSignal, mPointerLeaveSignal;
        Threading::Signal<const Input::PointerEventArgs &> mDragBeginSignal, mDragMoveSignal, mDragEndSignal;
        Threading::Signal<> mDragAbortSignal;
        Threading::Signal<const Input::AxisEventArgs &> mAxisEventSignal;
        Threading::Signal<const Input::KeyEventArgs &> mKeyPressSignal;

    private:
        WidgetBase *mParent;

        WidgetManager &mManager;

        std::vector<std::unique_ptr<WidgetBase>> mChildren;

        Vector2 mAbsolutePos;
        Vector3 mAbsoluteSize;

        PropertyList mProperties;

        Matrix3 mPos = Matrix3::ZERO;
        Matrix3 mSize = Matrix3::IDENTITY;
    };

    template <typename T>
    struct Widget : VirtualScope<T, Serialize::VirtualData<T, WidgetBase>> {

        using VirtualScope<T, Serialize::VirtualData<T, WidgetBase>>::VirtualScope;
    };
}
}

REGISTER_TYPE(Engine::Widgets::WidgetBase)
