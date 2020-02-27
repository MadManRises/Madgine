#pragma once

#include "Madgine/gui/toplevelwindowcomponentcollector.h"

#include "Modules/serialize/container/serializablecontainer.h"

#include "Modules/math/atlas2.h"

#include "imageloader.h"
#include "meshloader.h"
#include "programloader.h"
#include "textureloader.h"

#include "Madgine/render/renderpass.h"

#include "Modules/generic/transformIt.h"

#include "Madgine/render/shadinglanguage/sl.h"

#define SL_SHADER ui
#include INCLUDE_SL_SHADER

namespace Engine {
namespace Widgets {

    struct MADGINE_WIDGETS_EXPORT WidgetManager : GUI::TopLevelWindowComponent<WidgetManager>,
                                                 Render::RenderPass {

        SERIALIZABLEUNIT;

        WidgetManager(GUI::TopLevelWindow &window);
        WidgetManager(const WidgetManager &) = delete;
        ~WidgetManager();

        void swapCurrentRoot(WidgetBase *newRoot);
        void openModalWidget(WidgetBase *widget);
        void closeModalWidget(WidgetBase *widget);
        void openWidget(WidgetBase *widget);
        void closeWidget(WidgetBase *widget);

        bool isHovered(WidgetBase *w);
        WidgetBase *hoveredWidget();

        WidgetBase *getWidget(const std::string &name);

        void registerWidget(WidgetBase *w);
        void updateWidget(WidgetBase *w, const std::string &newName);
        void unregisterWidget(WidgetBase *w);

        virtual std::string_view key() const override;

        virtual bool init() override;
        virtual void finalize() override;

        WidgetBase *currentRoot();

        void clear();

        void destroyTopLevel(WidgetBase *w);
        WidgetBase *createTopLevelWidget(const std::string &name);
        Bar *createTopLevelBar(const std::string &name);
        Button *createTopLevelButton(const std::string &name);
        Checkbox *createTopLevelCheckbox(const std::string &name);
        Combobox *createTopLevelCombobox(const std::string &name);
        Image *createTopLevelImage(const std::string &name);
        Label *createTopLevelLabel(const std::string &name);
        SceneWindow *createTopLevelSceneWindow(const std::string &name);
        TabWidget *createTopLevelTabWidget(const std::string &name);
        Textbox *createTopLevelTextbox(const std::string &name);

        decltype(auto) widgets()
        {
            return uniquePtrToPtr(mTopLevelWidgets);
        }

        WidgetBase *mStartupWidget = nullptr;
        void openStartupWidget();

        virtual bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        virtual bool injectPointerRelease(const Input::PointerEventArgs &arg) override;
        virtual bool injectPointerMove(const Input::PointerEventArgs &arg) override;

        virtual void onResize(const Rect2i &space) override;
        virtual void render(Render::RenderTarget *target) override;

		virtual int priority() const override;

        Render::Texture &uiTexture() const;

    protected:
        WidgetBase *getHoveredWidget(const Vector2 &pos, const Rect2i &screenSpace, WidgetBase *current);
        WidgetBase *getHoveredWidgetDown(const Vector2 &pos, const Rect2i &screenSpace, WidgetBase *current);

        std::unique_ptr<WidgetBase> createWidgetClass(const std::string &name, WidgetClass _class);
        std::tuple<std::unique_ptr<WidgetBase>> createWidgetClassTuple(const std::string &name, WidgetClass _class);

        std::unique_ptr<WidgetBase> createWidget(const std::string &name);
        std::unique_ptr<Bar> createBar(const std::string &name);
        std::unique_ptr<Button> createButton(const std::string &name);
        std::unique_ptr<Checkbox> createCheckbox(const std::string &name);
        std::unique_ptr<Combobox> createCombobox(const std::string &name);
        std::unique_ptr<Image> createImage(const std::string &name);
        std::unique_ptr<Label> createLabel(const std::string &name);
        std::unique_ptr<SceneWindow> createSceneWindow(const std::string &name);
        std::unique_ptr<TabWidget> createTabWidget(const std::string &name);
        std::unique_ptr<Textbox> createTextbox(const std::string &name);

    private:
        std::map<std::string, WidgetBase *> mWidgets;

        SERIALIZABLE_CONTAINER(mTopLevelWidgets, std::vector<std::unique_ptr<WidgetBase>>);

        WidgetBase *mHoveredWidget = nullptr;

        WidgetBase *mCurrentRoot = nullptr;

        std::vector<WidgetBase *> mModalWidgetList;

        void expandUIAtlas();

        Render::ProgramLoader::HandleType mProgram;
        Render::MeshLoader::HandleType mMesh;

        Render::TextureLoader::HandleType mUIAtlasTexture;
        Atlas2 mUIAtlas;
        int mUIAtlasSize = 0;
        std::map<Resources::ImageLoader::ResourceType *, Atlas2::Entry> mUIAtlasEntries;

        WidgetsPerObject mParameters;
    };

}
}