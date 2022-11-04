#include "../widgetslib.h"

#include "widget.h"

#include "widgetmanager.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "textbox.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/fontloader/font.h"

METATABLE_BEGIN(Engine::Widgets::WidgetBase)
READONLY_PROPERTY(Children, children)
MEMBER(mName)
READONLY_PROPERTY(Pos, getPos)
READONLY_PROPERTY(Size, getSize)
MEMBER(mVisible)
METATABLE_END(Engine::Widgets::WidgetBase)

SERIALIZETABLE_BEGIN(Engine::Widgets::WidgetBase)
FIELD(mChildren, Serialize::ParentCreator<&Engine::Widgets::WidgetBase::readWidget, &Engine::Widgets::WidgetBase::writeWidget>)
FIELD(mName)
FIELD(mPos)
FIELD(mSize)
SERIALIZETABLE_END(Engine::Widgets::WidgetBase)

namespace Engine {

namespace Widgets {

    WidgetBase::WidgetBase(WidgetManager &manager, WidgetBase *parent)
        : mParent(parent)
        , mManager(manager)
    {
        mManager.registerWidget(this);
    }

    WidgetBase::~WidgetBase()
    {
        mManager.unregisterWidget(this);
    }

    void WidgetBase::setSize(const Matrix3 &size)
    {
        mSize = size;
        if (mParent)
            updateGeometry(mParent->getAbsoluteSize(), mParent->getAbsolutePosition());
        else
            updateGeometry(Vector3 { Vector2 { manager().getScreenSpace().mSize }, 1.0f });
    }

    const Matrix3 &WidgetBase::getSize()
    {
        return mSize;
    }

    void WidgetBase::setPos(const Matrix3 &pos)
    {
        mPos = pos;
        if (mParent)
            updateGeometry(mParent->getAbsoluteSize(), mParent->getAbsolutePosition());
        else
            updateGeometry(Vector3 { Vector2 { manager().getScreenSpace().mSize }, 1.0f });
    }

    const Matrix3 &WidgetBase::getPos() const
    {
        return mPos;
    }

    Vector3 WidgetBase::getAbsoluteSize() const
    {
        return mAbsoluteSize;
    }

    Vector2 WidgetBase::getAbsolutePosition() const
    {
        return mAbsolutePos;
    }

    void WidgetBase::updateGeometry(const Vector3 &parentSize, const Vector2 &parentPos)
    {
        mAbsoluteSize = mSize * parentSize;
        mAbsolutePos = (mPos * parentSize).xy() + parentPos;

        sizeChanged(mAbsoluteSize.floor());

        for (WidgetBase *child : uniquePtrToPtr(mChildren)) {
            child->updateGeometry(mAbsoluteSize, mAbsolutePos);
        }
    }

    WidgetClass WidgetBase::getClass() const
    {
        return WidgetClass::WIDGET;
    }

    void WidgetBase::destroy()
    {
        if (mParent)
            mParent->destroyChild(this);
        else
            mManager.destroyTopLevel(this);
    }

    const std::string &WidgetBase::key() const
    {
        return mName;
    }

    WidgetBase *WidgetBase::createChild(WidgetClass _class)
    {
        return mChildren.emplace_back(mManager.createWidgetByClass(_class, this)).get();
    }

    template <typename WidgetType>
    WidgetType *WidgetBase::createChild()
    {
        std::unique_ptr<WidgetType> p = mManager.create<WidgetType>(this);
        WidgetType *w = p.get();
        mChildren.emplace_back(std::move(p));
        return w;
    }

    template WidgetBase *WidgetBase::createChild<WidgetBase>();
    template Button *WidgetBase::createChild<Button>();
    template Bar *WidgetBase::createChild<Bar>();
    template Checkbox *WidgetBase::createChild<Checkbox>();
    template Label *WidgetBase::createChild<Label>();
    template Combobox *WidgetBase::createChild<Combobox>();
    template Textbox *WidgetBase::createChild<Textbox>();
    template SceneWindow *WidgetBase::createChild<SceneWindow>();
    template Image *WidgetBase::createChild<Image>();

    size_t WidgetBase::depth(size_t layer)
    {
        return mParent ? mParent->depth(layer) + 1 : 20 * layer;
    }

    WidgetManager &WidgetBase::manager()
    {
        return mManager;
    }

    bool WidgetBase::dragging() const
    {
        return mManager.dragging(this);
    }

    void WidgetBase::abortDrag()
    {
        mManager.abortDrag(this);
    }

    void WidgetBase::destroyChild(WidgetBase *w)
    {
        auto it = std::ranges::find(mChildren, w, projectionToRawPtr);
        assert(it != mChildren.end());
        mChildren.erase(it);
    }

    void WidgetBase::show()
    {
        mVisible = true;
    }

    void WidgetBase::hide()
    {
        mVisible = false;
    }

    bool WidgetBase::isFocused() const
    {
        return mManager.focusedWidget() == this;
    }

    WidgetBase *WidgetBase::getChildRecursive(std::string_view name)
    {
        if (name == mName || name.empty())
            return this;
        for (const std::unique_ptr<WidgetBase> &w : mChildren)
            if (WidgetBase *f = w->getChildRecursive(name))
                return f;
        return nullptr;
    }

    void WidgetBase::setParent(WidgetBase *parent)
    {
        if (mParent != parent) {
            auto it = std::ranges::find(mParent->mChildren, this, projectionToRawPtr);
            parent->mChildren.emplace_back(std::move(*it));
            mParent->mChildren.erase(it);
            mParent = parent;
            updateGeometry(parent->getAbsoluteSize(), parent->getAbsolutePosition());
        }
    }

    WidgetBase *WidgetBase::getParent() const
    {
        return mParent;
    }

    bool WidgetBase::injectPointerClick(const Input::PointerEventArgs &arg)
    {
        mPointerClickSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        mPointerMoveSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectPointerEnter(const Input::PointerEventArgs &arg)
    {
        mPointerEnterSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectPointerLeave(const Input::PointerEventArgs &arg)
    {
        mPointerLeaveSignal.emit(arg);
        return true;
    }

    bool Engine::Widgets::WidgetBase::injectDragBegin(const Input::PointerEventArgs &arg)
    {
        mDragBeginSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectDragMove(const Input::PointerEventArgs &arg)
    {
        mDragMoveSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectDragEnd(const Input::PointerEventArgs &arg)
    {
        mDragEndSignal.emit(arg);
        return true;
    }

    void WidgetBase::injectDragAbort()
    {
        mDragAbortSignal.emit();
    }

    bool WidgetBase::injectAxisEvent(const Input::AxisEventArgs &arg)
    {
        mAxisEventSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        mKeyPressSignal.emit(arg);
        return true;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerMoveEvent()
    {
        return mPointerMoveSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerClickEvent()
    {
        return mPointerClickSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerEnterEvent()
    {
        return mPointerEnterSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerLeaveEvent()
    {
        return mPointerLeaveSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::dragBeginEvent()
    {
        return mDragBeginSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::dragMoveEvent()
    {
        return mDragMoveSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::dragEndEvent()
    {
        return mDragEndSignal;
    }

    Threading::SignalStub<> &WidgetBase::dragAbortEvent()
    {
        return mDragAbortSignal;
    }

    Threading::SignalStub<const Input::AxisEventArgs &> &WidgetBase::axisEvent()
    {
        return mAxisEventSignal;
    }

    Threading::SignalStub<const Input::KeyEventArgs &> &WidgetBase::keyEvent()
    {
        return mKeyPressSignal;
    }

    bool WidgetBase::containsPoint(const Vector2 &point, const Rect2i &screenSpace, float extend) const
    {
        Vector2 min = mAbsolutePos + Vector2 { screenSpace.mTopLeft } - extend;
        Vector2 max = mAbsoluteSize.xy() + min + 2 * extend;
        return min.x <= point.x && min.y <= point.y && max.x >= point.x && max.y >= point.y;
    }

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> WidgetBase::vertices(const Vector3 &screenSize, size_t layer)
    {
        return {};
    }

    Serialize::StreamResult WidgetBase::readWidget(Serialize::FormattedSerializeStream &in, std::unique_ptr<WidgetBase> &widget)
    {
        return mManager.readWidget(in, widget, this);
    }

    const char *WidgetBase::writeWidget(Serialize::FormattedSerializeStream &out, const std::unique_ptr<WidgetBase> &widget) const
    {
        return mManager.writeWidget(out, widget);
    }

    void WidgetBase::sizeChanged(const Vector3i &pixelSize)
    {
    }

    void WidgetBase::preRender()
    {
    }

}
}
