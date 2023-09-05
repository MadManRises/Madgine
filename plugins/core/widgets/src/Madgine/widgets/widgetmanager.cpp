#include "../widgetslib.h"

#include "widgetmanager.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/meshloader/gpumeshloader.h"
#include "Madgine/pipelineloader/pipelineloader.h"

#include "widget.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "tablewidget.h"
#include "textbox.h"
#include "tabbar.h"
#include "textedit.h"

#include "Madgine/imageloader/imagedata.h"

#include "Madgine/window/mainwindow.h"

#include "Meta/math/atlas2.h"

#include "Interfaces/input/inputevents.h"

#include "Madgine/imageloader/imageloader.h"
#include "Madgine/meshloader/meshloader.h"
#include "Madgine/textureloader/textureloader.h"

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendertarget.h"

#include "Meta/serialize/helper/typedobjectserialize.h"

#include "Generic/areaview.h"

#include "Madgine/render/shadinglanguage/sl_support_begin.h"
#include "shaders/widgets.sl"
#include "Madgine/render/shadinglanguage/sl_support_end.h"

UNIQUECOMPONENT(Engine::Widgets::WidgetManager)

METATABLE_BEGIN(Engine::Widgets::WidgetManager)
READONLY_PROPERTY(Widgets, widgets)
MEMBER(mStartupWidget)
METATABLE_END(Engine::Widgets::WidgetManager)

SERIALIZETABLE_BEGIN(Engine::Widgets::WidgetManager)
FIELD(mStartupWidget)
FIELD(mTopLevelWidgets, Serialize::ParentCreator<&Engine::Widgets::WidgetManager::readWidgetStub, &Engine::Widgets::WidgetManager::writeWidget>)
SERIALIZETABLE_END(Engine::Widgets::WidgetManager)

namespace Engine {
namespace Widgets {

    static constexpr const char *sTags[] = {
        "Widget",
        "SceneWindow",
        "Combobox",
        "Checkbox",
        "Textbox",
        "Button",
        "Label",
        "Bar",
        "Image",
        "TableWidget",
        "TabBar",
        "TextEdit"
    };

    static float sDragDistanceThreshold = 2.0f;
    static std::chrono::steady_clock::duration sDragTimeThreshold = 5ms;

    struct WidgetManager::WidgetManagerData {

        Render::PipelineLoader::Instance mPipeline;
        Render::GPUMeshLoader::Ptr mMesh;

        Resources::ImageLoader::Handle mDefaultTexture;
        Render::TextureLoader::Ptr mUIAtlasTexture;
        std::set<Resources::ImageLoader::Handle> mImageLoadingTasks;
        Atlas2 mUIAtlas { { 2048, 2048 } };
        int mUIAtlasSize = 0;
        std::map<Resources::ImageLoader::Resource *, Atlas2::Entry> mUIAtlasEntries;

        void expandUIAtlas()
        {
            if (mUIAtlasSize == 0) {
                mUIAtlasSize = 1;
                mUIAtlasTexture.setData({ mUIAtlasSize * 2048, mUIAtlasSize * 2048 }, {});
                for (int x = 0; x < mUIAtlasSize; ++x) {
                    for (int y = 0; y < mUIAtlasSize; ++y) {
                        mUIAtlas.addBin({ 2048 * x, 2048 * y });
                    }
                }
            } else {
                /*for (int x = 0; x < mUIAtlasSize; ++x) {
                for (int y = 0; y < mUIAtlasSize; ++y) {
                    mUIAtlas.addBin({ 512 * x, 512 * (y + mUIAtlasSize) });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * y });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * (y + mUIAtlasSize) });
                }
            }
            mUIAtlasSize *= 2;
            mUIAtlasTexture.resize({ 512 * mUIAtlasSize, 512 * mUIAtlasSize });*/
                throw "TODO";
            }
        }
    };

    WidgetManager::WidgetManager(Window::MainWindow &window)
        : VirtualData(window, 20)
        , mData(std::make_unique<WidgetManagerData>())
    {
    }

    WidgetManager::~WidgetManager()
    {
        assert(mWidgets.empty());
    }

    std::string_view WidgetManager::key() const
    {
        return "WidgetManager";
    }

    Threading::Task<bool> WidgetManager::init()
    {
        if (!co_await MainWindowComponentBase::init())
            co_return false;

        mData->mPipeline.create({ .vs = "widgets", .ps = "widgets", .bufferSizes = { sizeof(WidgetsPerApplication), 0, sizeof(WidgetsPerObject) } });

        mData->mMesh.create({ 3, std::vector<Vertex> {} });

        mData->mUIAtlasTexture.create(Render::TextureType_2D, Render::FORMAT_RGBA8_SRGB);

        mData->mDefaultTexture.load("default_tex");

        co_return true;
    }

    Threading::Task<void> WidgetManager::finalize()
    {
        mTopLevelWidgets.clear();

        mData->mDefaultTexture.reset();

        mData->mUIAtlasTexture.reset();

        mData->mMesh.reset();

        mData->mPipeline.reset();

        co_await MainWindowComponentBase::finalize();

        co_return;
    }

    template <typename WidgetType>
    std::unique_ptr<WidgetType> WidgetManager::create(WidgetBase *parent)
    {
        std::unique_ptr<WidgetType> w = std::make_unique<WidgetType>(*this, parent);
        if (!parent) {
            w->hide();
        }
        w->applyGeometry(parent ? parent->getAbsoluteSize() : Vector3 { Vector2 { mClientSpace.mSize }, 1.0f });
        return w;
    }

    template std::unique_ptr<WidgetBase> WidgetManager::create<WidgetBase>(WidgetBase *);
    template std::unique_ptr<Bar> WidgetManager::create<Bar>(WidgetBase *);
    template std::unique_ptr<Checkbox> WidgetManager::create<Checkbox>(WidgetBase *);
    template std::unique_ptr<Label> WidgetManager::create<Label>(WidgetBase *);
    template std::unique_ptr<Button> WidgetManager::create<Button>(WidgetBase *);
    template std::unique_ptr<Combobox> WidgetManager::create<Combobox>(WidgetBase *);
    template std::unique_ptr<Textbox> WidgetManager::create<Textbox>(WidgetBase *);
    template std::unique_ptr<SceneWindow> WidgetManager::create<SceneWindow>(WidgetBase *);
    template std::unique_ptr<Image> WidgetManager::create<Image>(WidgetBase *);
    template std::unique_ptr<TableWidget> WidgetManager::create<TableWidget>(WidgetBase *);
    template std::unique_ptr<TextEdit> WidgetManager::create<TextEdit>(WidgetBase *);

    template <typename WidgetType>
    WidgetType *WidgetManager::createTopLevel()
    {
        std::unique_ptr<WidgetType> p = create<WidgetType>();
        WidgetType *w = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return w;
    }

    template DLL_EXPORT WidgetBase *WidgetManager::createTopLevel<WidgetBase>();

    std::unique_ptr<WidgetBase> WidgetManager::createWidgetByClass(WidgetClass _class, WidgetBase *parent)
    {
        switch (_class) {
        case WidgetClass::WIDGET:
            return create<WidgetBase>(parent);
        case WidgetClass::BAR:
            return create<Bar>(parent);
        case WidgetClass::CHECKBOX:
            return create<Checkbox>(parent);
        case WidgetClass::LABEL:
            return create<Label>(parent);
        case WidgetClass::BUTTON:
            return create<Button>(parent);
        case WidgetClass::COMBOBOX:
            return create<Combobox>(parent);
        case WidgetClass::TEXTBOX:
            return create<Textbox>(parent);
        case WidgetClass::SCENEWINDOW:
            return create<SceneWindow>(parent);
        case WidgetClass::IMAGE:
            return create<Image>(parent);
        case WidgetClass::TABLEWIDGET:
            return create<TableWidget>(parent);
        case WidgetClass::TABBAR:
            return create<TabBar>(parent);
        case WidgetClass::TEXTEDIT:
            return create<TextEdit>(parent);
        default:
            std::terminate();
        }
    }

    Serialize::StreamResult WidgetManager::readWidget(Serialize::FormattedSerializeStream &in, std::unique_ptr<WidgetBase> &widget, WidgetBase *parent)
    {
        WidgetClass _class;
        STREAM_PROPAGATE_ERROR(Serialize::beginExtendedTypedRead(in, _class, sTags));

        widget = createWidgetByClass(_class, parent);
        return {};
    }

    Serialize::StreamResult WidgetManager::readWidgetStub(Serialize::FormattedSerializeStream &in, std::unique_ptr<WidgetBase> &widget)
    {
        return readWidget(in, widget, nullptr);
    }

    const char *WidgetManager::writeWidget(Serialize::FormattedSerializeStream &out, const std::unique_ptr<WidgetBase> &widget) const
    {
        return Serialize::beginExtendedTypedWrite(out, widget->getClass(), sTags);
    }

    bool WidgetManager::injectPointerPress(const Input::PointerEventArgs &arg)
    {
        assert(mDragStartEvent.button != arg.button);
        if (mDragStartEvent.button != Input::MouseButton::NO_BUTTON)
            return true;

        if (mPointerEventTargetWidget) {
            mFocusedWidget = mPointerEventTargetWidget;

            mDragStartEvent = arg;

            Vector2i pos = mFocusedWidget->getAbsolutePosition().floor();
            mDragStartEvent.windowPosition = mDragStartEvent.windowPosition - InterfacesVector { pos.x, pos.y };

            mDragStartTime = std::chrono::steady_clock::now();

            return true;
        } else {
            mFocusedWidget = nullptr;
        }

        return false;
    }

    bool WidgetManager::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        for (WidgetBase *modalWidget : mModalWidgetList) {
            while (modalWidget) {
                if (modalWidget->injectKeyPress(arg))
                    return true;
                modalWidget = modalWidget->getParent();
            }
        }

        WidgetBase *w = mFocusedWidget;
        while (w) {
            if (w->injectKeyPress(arg))
                return true;
            w = w->getParent();
        }

        return false;
    }

    bool WidgetManager::injectPointerRelease(const Input::PointerEventArgs &arg)
    {
        if (mDragStartEvent.button != arg.button)
            return false;

        if (mFocusedWidget) {

            Vector2i pos = mFocusedWidget->getAbsolutePosition().floor();
            arg.windowPosition = arg.windowPosition - InterfacesVector { pos.x, pos.y };
            if (mDragging) {
                if (!mDraggingAborted)
                    mFocusedWidget->injectDragEnd(arg);
                mDragging = false;
            } else {
                mFocusedWidget->injectPointerClick(arg);
            }

            mDragStartEvent.button = Input::MouseButton::NO_BUTTON;

            return true;
        }

        return false;
    }

    WidgetBase *WidgetManager::getHoveredWidgetUp(const Vector2 &pos, WidgetBase *current)
    {
        if (!current) {
            return nullptr;
        } else if (!current->mVisible || !current->containsPoint(pos, { { 0, 0 }, mClientSpace.mSize })) {
            return getHoveredWidgetUp(pos, current->getParent());
        } else {
            return current;
        }
    }

    WidgetBase *WidgetManager::getHoveredWidgetDown(const Vector2 &pos, WidgetBase *current)
    {
        if (current) {
            for (WidgetBase *w : current->children()) {
                if (w->mVisible && w->containsPoint(pos, { { 0, 0 }, mClientSpace.mSize })) {
                    return getHoveredWidgetDown(pos, w);
                }
            }
        } else {
            if (!mModalWidgetList.empty()) {
                assert(mModalWidgetList.front()->mVisible);
                return getHoveredWidgetDown(pos, mModalWidgetList.front());
            }
            for (WidgetBase *w : widgets()) {
                if (w->mVisible && w->containsPoint(pos, { { 0, 0 }, mClientSpace.mSize })) {
                    return getHoveredWidgetDown(pos, w);
                }
            }
        }

        return current;
    }

    WidgetBase *WidgetManager::getHoveredWidget(const Vector2 &pos, WidgetBase *current)
    {
        return getHoveredWidgetDown(pos, getHoveredWidgetUp(pos, current));
    }

    bool WidgetManager::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        if (std::ranges::find(mWidgets, mHoveredWidget) == mWidgets.end())
            mHoveredWidget = nullptr;

        if (mDragStartEvent.button != Input::MouseButton::NO_BUTTON) {

            if (!mDragging) {
                InterfacesVector dist = arg.screenPosition - mDragStartEvent.screenPosition;
                if (std::abs(dist.x) + std::abs(dist.y) > sDragDistanceThreshold && std::chrono::steady_clock::now() - mDragStartTime > sDragTimeThreshold) {
                    mDragging = true;
                    mDraggingAborted = false;
                    mFocusedWidget->injectDragBegin(mDragStartEvent);
                }
            }

            if (mDragging && !mDraggingAborted) {
                Vector2i pos = mFocusedWidget->getAbsolutePosition().floor();
                arg.windowPosition = arg.windowPosition - InterfacesVector { pos.x, pos.y };
                mFocusedWidget->injectDragMove(arg);
            }

            return false;
        }

        WidgetBase *hoveredWidget = getHoveredWidget(Vector2 { Vector2i { &arg.windowPosition.x } }, mHoveredWidget);

        bool enter = false;
        if (mHoveredWidget != hoveredWidget) {

            if (mHoveredWidget) {
                InterfacesVector storedWindowPosition = arg.windowPosition;
                Vector2i pos = mHoveredWidget->getAbsolutePosition().floor();
                arg.windowPosition = arg.windowPosition - InterfacesVector { pos.x, pos.y };
                mHoveredWidget->injectPointerLeave(arg);
                arg.windowPosition = storedWindowPosition;
            }

            mHoveredWidget = hoveredWidget;
            enter = true;

            mPointerEventTargetWidget = hoveredWidget;
            while (mPointerEventTargetWidget && !mPointerEventTargetWidget->acceptsPointerEvents()) {
                mPointerEventTargetWidget = mPointerEventTargetWidget->getParent();
            }
        }

        if (mPointerEventTargetWidget) {
            Vector2i pos = mPointerEventTargetWidget->getAbsolutePosition().floor();
            arg.windowPosition = arg.windowPosition - InterfacesVector { pos.x, pos.y };

            if (enter)
                mPointerEventTargetWidget->injectPointerEnter(arg);

            mPointerEventTargetWidget->injectPointerMove(arg);
            return true;
        }

        return false;
    }

    bool WidgetManager::injectAxisEvent(const Input::AxisEventArgs &arg)
    {
        if (std::ranges::find(mWidgets, mHoveredWidget) == mWidgets.end())
            mHoveredWidget = nullptr;

        if (mHoveredWidget)
            return mHoveredWidget->injectAxisEvent(arg);

        return false;
    }

    WidgetBase *WidgetManager::currentRoot()
    {
        return mCurrentRoot;
    }

    void WidgetManager::destroyTopLevel(WidgetBase *w)
    {
        auto it = std::ranges::find(mTopLevelWidgets, w, projectionToRawPtr);
        assert(it != mTopLevelWidgets.end());
        mTopLevelWidgets.erase(it);
    }

    void WidgetManager::clear()
    {
        mTopLevelWidgets.clear();
    }

    bool WidgetManager::isHovered(WidgetBase *w)
    {
        WidgetBase *hovered = mHoveredWidget;
        while (hovered) {
            if (hovered == w)
                return true;
            hovered = hovered->getParent();
        }
        return false;
    }

    WidgetBase *WidgetManager::hoveredWidget()
    {
        return mHoveredWidget;
    }

    WidgetBase *WidgetManager::focusedWidget()
    {
        return mFocusedWidget;
    }

    WidgetBase *WidgetManager::pointerEventTargetWidget()
    {
        return mPointerEventTargetWidget;
    }

    WidgetBase *WidgetManager::getWidget(std::string_view name)
    {
        auto it = std::ranges::find(mWidgets, name, &WidgetBase::mName);
        if (it == mWidgets.end())
            return nullptr;
        return *it;
    }

    void WidgetManager::registerWidget(WidgetBase *w)
    {
        mWidgets.push_back(w);        
    }

    void WidgetManager::unregisterWidget(WidgetBase *w)
    {
        /* auto count = */std::erase(mWidgets, w);       
        //assert(count == 1);
    }

    void WidgetManager::resetPointerState()
    {
        mFocusedWidget = nullptr;
        mHoveredWidget = nullptr;
        if (mPointerEventTargetWidget) {
            Input::PointerEventArgs arg {
                { 0, 0 }, { 0, 0 }, { 0, 0 }
            };
            mPointerEventTargetWidget->injectPointerLeave(arg);
            mPointerEventTargetWidget = nullptr;
        }
    }

    void WidgetManager::swapCurrentRoot(std::string_view name)
    {
        auto it = std::ranges::find(mTopLevelWidgets, name, &WidgetBase::mName);
        if (it != mTopLevelWidgets.end())
            swapCurrentRoot(it->get());
    }

    void WidgetManager::swapCurrentRoot(WidgetBase *newRoot)
    {
        if (mCurrentRoot)
            mCurrentRoot->hide();

        resetPointerState();

        mCurrentRoot = newRoot;
        newRoot->show();
    }

    void WidgetManager::openModalWidget(WidgetBase *widget)
    {
        resetPointerState();

        mModalWidgetList.emplace(mModalWidgetList.begin(), widget);
        widget->show();
    }

    void WidgetManager::openWidget(WidgetBase *widget)
    {
        widget->show();
    }

    void WidgetManager::closeModalWidget(WidgetBase *widget)
    {
        resetPointerState();

        assert(mModalWidgetList.size() > 0 && mModalWidgetList.front() == widget);
        widget->hide();
        mModalWidgetList.erase(mModalWidgetList.begin());
    }

    void WidgetManager::closeWidget(WidgetBase *widget)
    {
        widget->hide();
    }

    void WidgetManager::openStartupWidget()
    {
        if (mStartupWidget)
            swapCurrentRoot(mStartupWidget);
        else if (mTopLevelWidgets.size() > 0)
            swapCurrentRoot(mTopLevelWidgets.front().get());
    }

    void WidgetManager::onResize(const Rect2i &space)
    {
        MainWindowComponentBase::onResize(space);
        for (WidgetBase *topLevel : widgets()) {
            topLevel->applyGeometry(Vector3 { Vector2 { space.mSize }, 1.0f });
        }
    }

    void WidgetManager::render(Render::RenderTarget *target, size_t iteration)
    {
        if (!mData->mPipeline.available())
            return;

        target->pushAnnotation("WidgetManager");

        MainWindowComponentBase::render(target, iteration);

        WidgetsRenderData renderData;
        auto keep = renderData.pushClipRect(Vector2::ZERO, Vector2 { mClientSpace.mSize });

        for (auto [w, layer] : visibleWidgets()) {
            w->vertices(renderData, layer);
        }

        {
            auto perApp = mData->mPipeline->mapParameters<WidgetsPerApplication>(0);
            perApp->c = target->getClipSpaceMatrix();
            perApp->screenSize = Vector2 { mClientSpace.mSize };
        }

        for (auto &[tex, vertexData] : renderData.vertexData()) {
            if (vertexData.mTriangleVertices.empty())
                continue;

            {
                auto parameters = mData->mPipeline->mapParameters<WidgetsPerObject>(2);
                parameters->hasDistanceField = bool(tex.mFlags & TextureFlag_IsDistanceField);
                parameters->hasTexture = true;
            }

            if (tex.mTexture.mTextureHandle)
                mData->mPipeline->bindTextures(target, { tex.mTexture });
            else
                mData->mPipeline->bindTextures(target, { mData->mUIAtlasTexture->descriptor() });

            mData->mMesh.update({ 3, std::move(vertexData.mTriangleVertices) });

            mData->mPipeline->renderMesh(target, mData->mMesh);
        }
        if (!renderData.lineVertices().empty()) {
            {
                auto parameters = mData->mPipeline->mapParameters<WidgetsPerObject>(2);
                parameters->hasDistanceField = false;
                parameters->hasTexture = false;
            }

            mData->mPipeline->bindTextures(target, { mData->mUIAtlasTexture->descriptor() });

            mData->mMesh.update({ 2, std::move(renderData.lineVertices()) });

            mData->mPipeline->renderMesh(target, mData->mMesh);
        }

        target->popAnnotation();
    }

    Generator<std::pair<WidgetBase *, size_t>> WidgetManager::visibleWidgets()
    {
        std::queue<std::pair<Widgets::WidgetBase *, size_t>> q;
        if (mCurrentRoot)
            q.emplace(mCurrentRoot, 0);
        int layer = 0;
        for (Widgets::WidgetBase *w : mModalWidgetList) {
            if (w->mVisible) {
                q.emplace(w, ++layer);
            }
        }
        while (!q.empty()) {
            auto [w, l] = q.front();
            q.pop();

            for (Widgets::WidgetBase *c : w->children()) {
                if (c->mVisible)
                    q.emplace(c, l);
            }

            co_yield { w, l };
        }
    }

    const Render::Texture &WidgetManager::uiTexture() const
    {
        return *mData->mUIAtlasTexture;
    }

    Threading::SignalStub<> &WidgetManager::updatedSignal()
    {
        return mUpdatedSignal;
    }

    void WidgetManager::onActivate(bool active)
    {
        if (active) {
            openStartupWidget();
            mUpdatedSignal.emit();
        }
    }

    const Atlas2::Entry *WidgetManager::lookUpImage(Resources::ImageLoader::Resource *image)
    {
        if (!image)
            return nullptr;
        auto it = mData->mUIAtlasEntries.find(image);
        if (it == mData->mUIAtlasEntries.end()) {
            Resources::ImageLoader::Handle data = image->loadData();
            if (!data.available()) {
                mData->mImageLoadingTasks.emplace(std::move(data));
                return nullptr;
            } else {

                size_t width = data->mSize.x;
                size_t height = data->mSize.y;

                assert(data->mChannels == 4);

                const uint32_t *source = static_cast<const uint32_t *>(data->mBuffer.mData);

                std::vector<uint32_t> targetBuffer;
                targetBuffer.resize((width + 2) * (height + 2));

                AreaView<uint32_t, 2> targetArea { targetBuffer.data(), { width + 2, height + 2 } };

                AreaView<const uint32_t, 2> sourceArea { source, { width, height } };

                std::ranges::copy(sourceArea, targetArea.subArea({ 1, 1 }, { width, height }).begin());
                std::ranges::copy(sourceArea.subArea({ 0, 0 }, { 1, height }), targetArea.subArea({ 0, 1 }, { 1, height }).begin());
                std::ranges::copy(sourceArea.subArea({ width - 1, 0 }, { 1, height }), targetArea.subArea({ width + 1, 1 }, { 1, height }).begin());
                std::ranges::copy(sourceArea.subArea({ 0, 0 }, { width, 1 }), targetArea.subArea({ 1, 0 }, { width, 1 }).begin());
                std::ranges::copy(sourceArea.subArea({ 0, height - 1 }, { width, 1 }), targetArea.subArea({ 1, height + 1 }, { width, 1 }).begin());

                targetArea[0][0] = sourceArea[0][0];
                targetArea[0][width + 1] = sourceArea[0][width - 1];
                targetArea[height + 1][0] = sourceArea[height - 1][0];
                targetArea[height + 1][width + 1] = sourceArea[height - 1][width - 1];

                Atlas2::Entry entry = mData->mUIAtlas.insert(data->mSize + Vector2i { 2, 2 }, [this]() { mData->expandUIAtlas(); });
                it = mData->mUIAtlasEntries.try_emplace(image, entry).first;
                mData->mUIAtlasTexture.setSubData(entry.mArea.mTopLeft, data->mSize + Vector2i { 2, 2 }, targetBuffer);
                mData->mImageLoadingTasks.erase(data);
            }
        }
        return &it->second;
    }

    const Atlas2::Entry *WidgetManager::lookUpImage(std::string_view name)
    {
        return lookUpImage(Resources::ImageLoader::getSingleton().get(name));
    }

    bool WidgetManager::dragging(const WidgetBase *widget)
    {
        return mFocusedWidget == widget && mDragging && !mDraggingAborted;
    }

    void WidgetManager::abortDrag(WidgetBase *widget)
    {
        if (dragging(widget)) {
            mFocusedWidget->injectDragAbort();
            mDraggingAborted = true;
        }
    }

}
}