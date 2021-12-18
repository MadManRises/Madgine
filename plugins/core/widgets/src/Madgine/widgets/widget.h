#pragma once

#include "Meta/math/matrix3.h"

#include "Modules/threading/signal.h"
#include "widgetclass.h"

#include "Generic/container/transformIt.h"

#include "Meta/serialize/virtualserializableunit.h"

#include "Meta/keyvalue/virtualscope.h"

#include "render/texturedescriptor.h"
#include "vertex.h"

namespace Engine {
namespace Widgets {
    typedef int TextureFlags;
    enum TextureFlags_ {
        TextureFlag_IsDistanceField = 1 << 0
    };

    struct TextureSettings {
        Render::TextureDescriptor mTexture;
        TextureFlags mFlags = 0;

        auto operator<=>(const TextureSettings &other) const = default;
    };

    struct MADGINE_WIDGETS_EXPORT WidgetBase : VirtualScope<WidgetBase, Serialize::VirtualData<WidgetBase, Serialize::VirtualSerializableDataBase<VirtualScopeBase<>, Serialize::SerializableDataUnit>>> {
        SERIALIZABLEUNIT(WidgetBase);        

        WidgetBase(const std::string &name, WidgetManager &manager, WidgetBase *parent = nullptr);

        WidgetBase(const WidgetBase &) = delete;

        virtual ~WidgetBase();

        void setSize(const Matrix3 &size);
        const Matrix3 &getSize();
        void setPos(const Matrix3 &pos);
        const Matrix3 &getPos() const;

        Matrix3 getAbsoluteSize() const;
        Matrix3 getAbsolutePosition() const;

        Vector3 getActualSize() const;
        Vector3 getActualPosition() const;

        void updateGeometry(const Rect2i &screenSpace, const Matrix3 &parentSize = Matrix3::IDENTITY, const Matrix3 &parentPos = Matrix3::ZERO);

        virtual WidgetClass getClass() const;

        void destroy();

        void show();
        void hide();

        virtual void setEnabled(bool b);

        const std::string &getName() const;
        void setName(const std::string &name);

        const std::string &key() const;

        WidgetBase *createChild(const std::string &name, WidgetClass _class);
        template <typename WidgetType = WidgetBase>
        WidgetType *createChild(const std::string &name);

        WidgetBase *getChildRecursive(const std::string &name);
        template <typename T>
        T* getChildRecursive(const std::string& name) {
            return dynamic_cast<T *>(getChildRecursive(name));
        }
        void setParent(WidgetBase *parent);
        WidgetBase *getParent() const;

        virtual bool injectPointerPress(const Input::PointerEventArgs &arg);
        virtual bool injectPointerRelease(const Input::PointerEventArgs &arg);
        virtual bool injectPointerMove(const Input::PointerEventArgs &arg);
        virtual bool injectPointerEnter(const Input::PointerEventArgs &arg);
        virtual bool injectPointerLeave(const Input::PointerEventArgs &arg);
        virtual bool injectAxisEvent(const Input::AxisEventArgs &arg);

        Threading::SignalStub<const Input::PointerEventArgs &> &pointerMoveEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerDownEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerUpEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerEnterEvent();
        Threading::SignalStub<const Input::PointerEventArgs &> &pointerLeaveEvent();
        Threading::SignalStub<const Input::AxisEventArgs &> &axisEvent();

        decltype(auto) children() const
        {
            return uniquePtrToPtr(static_cast<const std::vector<std::unique_ptr<WidgetBase>> &>(mChildren));
        }

        bool containsPoint(const Vector2 &point, const Rect2i &screenSpace, float extend = 0.0f) const;

        virtual std::vector<std::pair<std::vector<Vertex>, TextureSettings>> vertices(const Vector3 &screenSize);
        virtual void preRender();

        void *userData();
        void setUserData(void *userData);

        virtual Resources::ResourceType<Resources::ImageLoader> *resource() const;

        size_t depth();

        bool mVisible = true;

        WidgetManager &manager();

    protected:
        std::unique_ptr<WidgetBase> createWidgetClass(const std::string &name, WidgetClass _class);
        std::tuple<std::unique_ptr<WidgetBase>> createWidgetClassTuple(const std::string &name, WidgetClass _class);
        std::tuple<std::string, WidgetClass> storeWidgetCreationData(const std::unique_ptr<WidgetBase> &widget) const;

        virtual void sizeChanged(const Vector3i &pixelSize);

        std::pair<std::vector<Vertex>, TextureSettings> renderText(const std::string &text, Vector3 pos, const Render::Font *font, float fontSize, Vector2 pivot, const Vector3 &screenSize);

    protected:
        void destroyChild(WidgetBase *w);

        Threading::Signal<const Input::PointerEventArgs &> mPointerMoveSignal, mPointerDownSignal, mPointerUpSignal, mPointerEnterSignal, mPointerLeaveSignal;
        Threading::Signal<const Input::AxisEventArgs &> mAxisEventSignal;

    private:
        std::string mName;
        WidgetBase *mParent;

        WidgetManager &mManager;

        std::vector<std::unique_ptr<WidgetBase>> mChildren;

        Matrix3 mPos = Matrix3::ZERO;
        Matrix3 mSize = Matrix3::IDENTITY;

        Matrix3 mAbsolutePos, mAbsoluteSize;

        void *mUserData = nullptr;
    };

    template <typename T>
    struct Widget : VirtualScope<T, Serialize::VirtualData<T, WidgetBase>> {

        using VirtualScope<T, Serialize::VirtualData<T, WidgetBase>>::VirtualScope;
    };
}
}

RegisterType(Engine::Widgets::WidgetBase);
