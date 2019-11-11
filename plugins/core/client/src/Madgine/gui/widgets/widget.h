#pragma once

#include "Modules/math/matrix3.h"

#include "../../input/inputevents.h"

#include "Modules/signalslot/signal.h"
#include "widgetclass.h"

#include "Modules/generic/transformIt.h"

#include "Modules/keyvalue/scopebase.h"

#include "Modules/keyvalue/metatable.h"

#include "imageloaderlib.h"
#include "imageloader.h"

#include "Modules/serialize/container/serializablecontainer.h"

#include "../../render/texturedescriptor.h"

#include "../vertex.h"

namespace Engine {
namespace Widgets {
    class MADGINE_CLIENT_EXPORT WidgetBase : public ScopeBase,
                                             public Serialize::SerializableUnit<WidgetBase> {
        SERIALIZABLEUNIT;

    public:
        WidgetBase(const std::string &name, WidgetBase *parent);

        WidgetBase(const std::string &name, WidgetManager &manager);

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
        void screenSpaceChanged(const Rect2i &screenSpace);

        virtual WidgetClass getClass() const;

        void destroy();

        void show();
        void hide();

        virtual void setEnabled(bool b);

        const std::string &getName() const;
        void setName(const std::string &name);

        const char *key() const;

        WidgetBase *createChild(const std::string &name, WidgetClass _class);
        WidgetBase *createChildWidget(const std::string &name);
        Bar *createChildBar(const std::string &name);
        Button *createChildButton(const std::string &name);
        Checkbox *createChildCheckbox(const std::string &name);
        Combobox *createChildCombobox(const std::string &name);
        Label *createChildLabel(const std::string &name);
        SceneWindow *createChildSceneWindow(const std::string &name);
        TabWidget *createChildTabWidget(const std::string &name);
        Textbox *createChildTextbox(const std::string &name);
        Image *createChildImage(const std::string &name);

        WidgetBase *getChildRecursive(const std::string &name);
        void setParent(WidgetBase *parent);
        WidgetBase *getParent() const;

        virtual bool injectPointerPress(const Input::PointerEventArgs &arg);
        virtual bool injectPointerRelease(const Input::PointerEventArgs &arg);
        virtual bool injectPointerMove(const Input::PointerEventArgs &arg);
        virtual bool injectPointerEnter(const Input::PointerEventArgs &arg);
        virtual bool injectPointerLeave(const Input::PointerEventArgs &arg);

        SignalSlot::SignalStub<const Input::PointerEventArgs &> &pointerMoveEvent();
        SignalSlot::SignalStub<const Input::PointerEventArgs &> &pointerDownEvent();
        SignalSlot::SignalStub<const Input::PointerEventArgs &> &pointerUpEvent();
        SignalSlot::SignalStub<const Input::PointerEventArgs &> &pointerEnterEvent();
        SignalSlot::SignalStub<const Input::PointerEventArgs &> &pointerLeaveEvent();

        decltype(auto) children() const
        {
            return uniquePtrToPtr(static_cast<const std::vector<std::unique_ptr<WidgetBase>> &>(mChildren));
        }

        bool containsPoint(const Vector2 &point, const Rect2i &screenSpace, float extend = 0.0f) const;

        virtual std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> vertices(const Vector3 &screenSize);

        void *userData();
        void setUserData(void *userData);

        virtual const MetaTable *type();

        virtual Resources::ImageLoader::ResourceType *resource() const;

        void writeCreationData(Serialize::SerializeOutStream &of) const;

		size_t depth();

		bool mVisible = true;

    protected:
        std::unique_ptr<WidgetBase> createWidgetClass(const std::string &name, WidgetClass _class);
        std::tuple<std::unique_ptr<WidgetBase>> createWidgetClassTuple(const std::string &name, WidgetClass _class);

        virtual std::unique_ptr<WidgetBase> createWidget(const std::string &name);
        virtual std::unique_ptr<Bar> createBar(const std::string &name);
        virtual std::unique_ptr<Button> createButton(const std::string &name);
        virtual std::unique_ptr<Checkbox> createCheckbox(const std::string &name);
        virtual std::unique_ptr<Combobox> createCombobox(const std::string &name);
        virtual std::unique_ptr<Image> createImage(const std::string &name);
        virtual std::unique_ptr<Label> createLabel(const std::string &name);
        virtual std::unique_ptr<SceneWindow> createSceneWindow(const std::string &name);
        virtual std::unique_ptr<TabWidget> createTabWidget(const std::string &name);
        virtual std::unique_ptr<Textbox> createTextbox(const std::string &name);

        virtual void sizeChanged(const Vector3i &pixelSize);

		std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor> renderText(const std::string &text, Vector3 pos, Font::Font *font, float fontSize, Vector2 pivot, const Vector3 &screenSize);

    protected:

        WidgetManager &manager();

        void destroyChild(WidgetBase *w);

        //KeyValueMapList maps() override;

        SignalSlot::Signal<const Input::PointerEventArgs &> mPointerMoveSignal, mPointerDownSignal, mPointerUpSignal, mPointerEnterSignal, mPointerLeaveSignal;

    private:
        std::string mName;
        WidgetBase *mParent;

        WidgetManager &mManager;

        SERIALIZABLE_CONTAINER(mChildren, std::vector<std::unique_ptr<WidgetBase>>);

        Matrix3 mPos = Matrix3::ZERO;
        Matrix3 mSize = Matrix3::IDENTITY;

        Matrix3 mAbsolutePos, mAbsoluteSize;

        void *mUserData = nullptr;
    };

    template <typename T>
    struct Widget : Serialize::SerializableUnit<T, WidgetBase> {

        using Serialize::SerializableUnit<T, WidgetBase>::SerializableUnit;

        virtual const MetaTable *type() override
        {
            return &table<T>();
        }
    };
}
}
