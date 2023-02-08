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

#include "geometry.h"

METATABLE_BEGIN(Engine::Widgets::WidgetBase)
READONLY_PROPERTY(Children, children)
MEMBER(mName)
READONLY_PROPERTY(Pos, getPos)
READONLY_PROPERTY(Size, getSize)
MEMBER(mVisible)
MEMBER(mConditions)
METATABLE_END(Engine::Widgets::WidgetBase)

SERIALIZETABLE_BEGIN(Engine::Widgets::WidgetBase)
FIELD(mChildren, Serialize::ParentCreator<&Engine::Widgets::WidgetBase::readWidget, &Engine::Widgets::WidgetBase::writeWidget>)
FIELD(mName)
FIELD(mPos)
FIELD(mSize)
FIELD(mConditions)
FIELD(mProperties)
SERIALIZETABLE_END(Engine::Widgets::WidgetBase)

namespace Engine {

namespace Widgets {

    WidgetBase::WidgetBase(WidgetManager &manager, WidgetBase *parent, bool acceptsPointerEvents)
        : mManager(manager)
        , mParent(parent)
        , mAcceptsPointerEvents(acceptsPointerEvents || !parent)
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
        applyGeometry();
    }

    const Matrix3 &WidgetBase::getSize()
    {
        return mSize;
    }

    void WidgetBase::setPos(const Matrix3 &pos)
    {
        mPos = pos;
        applyGeometry();
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

    void WidgetBase::applyGeometry()
    {
        if (mParent)
            applyGeometry(mParent->getAbsoluteSize(), mParent->getAbsolutePosition());
        else
            applyGeometry(Vector3 { Vector2 { manager().getScreenSpace().mSize }, 1.0f });
    }

    void WidgetBase::applyGeometry(const Vector3 &parentSize, const Vector2 &parentPos)
    {
        Geometry geometry = calculateGeometry(fetchActiveConditions());

        mAbsoluteSize = geometry.mSize * parentSize;
        mAbsolutePos = (geometry.mPos * parentSize).xy() + parentPos;

        sizeChanged(mAbsoluteSize);

        for (WidgetBase *child : children()) {
            child->applyGeometry(mAbsoluteSize, mAbsolutePos);
        }
    }

    void applyProperties(Geometry &geometry, PropertyRange range, uint16_t activeConditions, uint16_t rangeMask, GeometrySourceInfo *source)
    {

        for (auto it = range.begin(); it != range.end(); ++it) {
            const PropertyDescriptor &desc = *it;
            switch (desc.mType) {
            case PropertyType::POSITION:
                geometry.mPos[desc.mAnnotator1 / 3][desc.mAnnotator1 % 3] = it.value(0);
                if (source)
                    source->mPos[desc.mAnnotator1] = rangeMask;
                break;
            case PropertyType::SIZE:
                geometry.mSize[desc.mAnnotator1 / 3][desc.mAnnotator1 % 3] = it.value(0);
                if (source)
                    source->mSize[desc.mAnnotator1] = rangeMask;
                break;
            case PropertyType::CONDITIONAL:
                assert(rangeMask == 0);
                if ((activeConditions & desc.mAnnotator1) == desc.mAnnotator1)
                    applyProperties(geometry, it.conditionalRange(), activeConditions, desc.mAnnotator1, source);
                break;
            }
        }

    }

    Geometry WidgetBase::calculateGeometry(uint16_t activeConditions, GeometrySourceInfo *source)
    {
        Geometry geometry;
        geometry.mPos = getPos();
        geometry.mSize = getSize();

        applyProperties(geometry, mProperties, activeConditions, 0, source);

        return geometry;
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

    void WidgetBase::setVisible(bool v)
    {
        mVisible = v;
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
            applyGeometry(parent->getAbsoluteSize(), parent->getAbsolutePosition());
        }
    }

    WidgetBase *WidgetBase::getParent() const
    {
        return mParent;
    }

    void WidgetBase::setAcceptsPointerEvents(bool v)
    {
        mAcceptsPointerEvents = v;
    }

    bool WidgetBase::acceptsPointerEvents() const
    {
        return mAcceptsPointerEvents;
    }

    void WidgetBase::injectPointerClick(const Input::PointerEventArgs &arg)
    {
        mPointerClickSignal.emit(arg);
    }

    void WidgetBase::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        mPointerMoveSignal.emit(arg);
    }

    void WidgetBase::injectPointerEnter(const Input::PointerEventArgs &arg)
    {
        mPointerEnterSignal.emit(arg);        
    }

    void WidgetBase::injectPointerLeave(const Input::PointerEventArgs &arg)
    {
        mPointerLeaveSignal.emit(arg);        
    }

    void WidgetBase::injectDragBegin(const Input::PointerEventArgs &arg)
    {
        mDragBeginSignal.emit(arg);
    }

    void WidgetBase::injectDragMove(const Input::PointerEventArgs &arg)
    {
        mDragMoveSignal.emit(arg);
    }

    void WidgetBase::injectDragEnd(const Input::PointerEventArgs &arg)
    {
        mDragEndSignal.emit(arg);        
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

    void WidgetBase::vertices(WidgetsRenderData &renderData, size_t layer)
    {
    }

    Serialize::StreamResult WidgetBase::readWidget(Serialize::FormattedSerializeStream &in, std::unique_ptr<WidgetBase> &widget)
    {
        return mManager.readWidget(in, widget, this);
    }

    const char *WidgetBase::writeWidget(Serialize::FormattedSerializeStream &out, const std::unique_ptr<WidgetBase> &widget) const
    {
        return mManager.writeWidget(out, widget);
    }

    void WidgetBase::sizeChanged(const Vector3 &pixelSize)
    {
    }

    uint16_t WidgetBase::fetchActiveConditions(std::vector<Condition *> *conditions)
    {
        if (!mParent)
            return 0;
        if (conditions) {
            return mParent->fetchActiveConditionsImpl(*conditions);
        } else {
            std::vector<Condition *> conditionsDummy;
            return mParent->fetchActiveConditionsImpl(conditionsDummy);
        }
    }

    uint16_t WidgetBase::fetchActiveConditionsImpl(std::vector<Condition *> &conditions)
    {
        uint16_t acc = 0;

        if (mParent)
            acc = mParent->fetchActiveConditionsImpl(conditions);

        for (Condition &cond : mConditions) {
            acc |= evalCondition(cond) << conditions.size();
            conditions.push_back(&cond);
        }

        return acc;
    }

    bool WidgetBase::evalCondition(Condition &cond)
    {
        float formulaValue;
        switch (cond.mFormula) {
        case Formula::W:
            formulaValue = mAbsoluteSize.x;
            break;
        case Formula::H:
            formulaValue = mAbsoluteSize.y;
            break;
        case Formula::W_MINUS_H:
            formulaValue = mAbsoluteSize.x - mAbsoluteSize.y;
            break;
        case Formula::ABS_W_MINUS_H:
            formulaValue = abs(mAbsoluteSize.x - mAbsoluteSize.y);
            break;
        case Formula::W_OVER_H:
            formulaValue = mAbsoluteSize.x / mAbsoluteSize.y;
            break;
        default:
            throw 0;
        }
        switch (cond.mOperator) {
        case Operator::LESS:
            return formulaValue < cond.mReferenceValue;
        case Operator::GREATER:
            return formulaValue > cond.mReferenceValue;
        case Operator::LESS_OR_EQUAL:
            return formulaValue <= cond.mReferenceValue;
        case Operator::GREATER_OR_EQUAL:
            return formulaValue >= cond.mReferenceValue;
        default:
            throw 0;
        }
    }

    void WidgetBase::addConditional(uint16_t mask)
    {
        mProperties.set(PropertyDescriptor { PropertyType::CONDITIONAL, 0, mask });
    }

    PropertyRange WidgetBase::conditionals()
    {
        return {
            std::ranges::find_if(mProperties, [](const PropertyDescriptor &prop) { return prop.mType == PropertyType::CONDITIONAL; }), mProperties.end()
        };
    }

    void WidgetBase::setPosValue(uint16_t index, float value, uint16_t mask)
    {
        if (mask == 0) {
            mPos[index / 3][index % 3] = value;
        }
        else {
            mProperties.setConditional(mask, { PropertyType::POSITION, 0, index }, { value });
        }
        applyGeometry();
    }

    void WidgetBase::unsetPosValue(uint16_t index, uint16_t mask)
    {
        if (mask == 0) {
            LOG_WARNING("Unsetting a pos value without conditional has no effect.");
        } else {
            mProperties.unsetConditional(mask, { PropertyType::POSITION, 0, index });
        }
        applyGeometry();
    }

    void WidgetBase::setSizeValue(uint16_t index, float value, uint16_t mask)
    {
        if (mask == 0) {
            mSize[index / 3][index % 3] = value;
        }
        else {
            mProperties.setConditional(mask, { PropertyType::SIZE, 0, index }, { value });
        }
        applyGeometry();
    }

    void WidgetBase::unsetSizeValue(uint16_t index, uint16_t mask)
    {
        if (mask == 0) {
            LOG_WARNING("Unsetting a size value without conditional has no effect.");
        } else {
            mProperties.unsetConditional(mask, { PropertyType::SIZE, 0, index });
        }
        applyGeometry();
    }

}
}
