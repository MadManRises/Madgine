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

namespace Engine {
namespace Widgets {

    struct MADGINE_WIDGETS_EXPORT WidgetBase : VirtualScope<WidgetBase, Serialize::VirtualData<WidgetBase, Serialize::VirtualSerializableDataBase<VirtualScopeBase<>, Serialize::SerializableDataUnit>>> {
        SERIALIZABLEUNIT(WidgetBase)

        WidgetBase(WidgetManager &manager, WidgetBase *parent = nullptr);

        WidgetBase(const WidgetBase &) = delete;

        virtual ~WidgetBase();

        void setSize(const Matrix3 &size);
        const Matrix3 &getSize();
        void setPos(const Matrix3 &pos);
        const Matrix3 &getPos() const;

        Matrix3 getEffectiveSize() const;
        Matrix3 getEffectivePosition() const;

        Vector3 getAbsoluteSize() const;
        Vector3 getAbsolutePosition() const;

        void updateGeometry(const Rect2i &screenSpace, const Matrix3 &parentSize = Matrix3::IDENTITY, const Matrix3 &parentPos = Matrix3::ZERO);

        virtual WidgetClass getClass() const;

        void destroy();

        void show();
        void hide();

        virtual void setEnabled(bool b);

        bool isFocused() const;

        const std::string &key() const;

        WidgetBase *createChild(WidgetClass _class);
        template <typename WidgetType = WidgetBase>
        MADGINE_WIDGETS_EXPORT WidgetType *createChild();

        WidgetBase *getChildRecursive(std::string_view name);
        template <typename T>
        T *getChildRecursive(std::string_view name)
        {
            return dynamic_cast<T *>(getChildRecursive(name));
        }
        void setParent(WidgetBase *parent);
        WidgetBase *getParent() const;

        virtual bool injectPointerClick(const Input::PointerEventArgs &arg);
        virtual bool injectPointerMove(const Input::PointerEventArgs &arg);
        virtual bool injectPointerEnter(const Input::PointerEventArgs &arg);
        virtual bool injectPointerLeave(const Input::PointerEventArgs &arg);
        virtual bool injectDragBegin(const Input::PointerEventArgs &arg);
        virtual bool injectDragMove(const Input::PointerEventArgs &arg);
        virtual bool injectDragEnd(const Input::PointerEventArgs &arg);
        virtual bool injectAxisEvent(const Input::AxisEventArgs &arg);
        virtual bool injectKeyPress(const Input::KeyEventArgs &arg);

        Threading::SignalStub<const Input::PointerEventArgs &> &pointerMoveEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerClickEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerEnterEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerLeaveEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &dragBeginEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &dragMoveEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &dragEndEvent();
        Threading::SignalStub<const Input::AxisEventArgs &> &axisEvent();
        Threading::SignalStub<const Input::KeyEventArgs &> &keyEvent();

        decltype(auto) children() const
        {
            return uniquePtrToPtr(static_cast<const std::vector<std::unique_ptr<WidgetBase>> &>(mChildren));
        }

        bool containsPoint(const Vector2 &point, const Rect2i &screenSpace, float extend = 0.0f) const;

        virtual std::vector<std::pair<std::vector<Vertex>, TextureSettings>> vertices(const Vector3 &screenSize);
        virtual void preRender();

        void *userData();
        void setUserData(void *userData);

        size_t depth();

        bool mVisible = true;
        std::string mName = "Unnamed";

        WidgetManager &manager();

    protected:
        std::unique_ptr<WidgetBase> createWidgetByClass(WidgetClass _class);
        Serialize::StreamResult readWidget(Serialize::FormattedSerializeStream &in, std::unique_ptr<WidgetBase> &widget);
        const char *writeWidget(Serialize::FormattedSerializeStream &out, const std::unique_ptr<WidgetBase> &widget) const;

        virtual void sizeChanged(const Vector3i &pixelSize);

    protected:
        void destroyChild(WidgetBase *w);

        Threading::Signal<const Input::PointerEventArgs &> mPointerMoveSignal, mPointerClickSignal, mPointerEnterSignal, mPointerLeaveSignal;
        Threading::Signal<const Input::PointerEventArgs &> mDragBeginSignal, mDragMoveSignal, mDragEndSignal;
        Threading::Signal<const Input::AxisEventArgs &> mAxisEventSignal;
        Threading::Signal<const Input::KeyEventArgs &> mKeyPressSignal;

    private:
        WidgetBase *mParent;

        WidgetManager &mManager;

        std::vector<std::unique_ptr<WidgetBase>> mChildren;

        Matrix3 mPos = Matrix3::ZERO;
        Matrix3 mSize = Matrix3::IDENTITY;

        Matrix3 mEffectivePos, mEffectiveSize;

        void *mUserData = nullptr;
    };

    template <typename T>
    struct Widget : VirtualScope<T, Serialize::VirtualData<T, WidgetBase>> {

        using VirtualScope<T, Serialize::VirtualData<T, WidgetBase>>::VirtualScope;
    };
}
}

REGISTER_TYPE(Engine::Widgets::WidgetBase)
