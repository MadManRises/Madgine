#pragma once

#include "../../input/inputcollector.h"
#include "../../input/inputlistener.h"
#include "Interfaces/window/windoweventlistener.h"
#include "Modules/uniquecomponent/uniquecomponentdefine.h"
#include "Modules/uniquecomponent/uniquecomponentselector.h"

#include "Modules/generic/transformIt.h"

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalue/scopebase.h"

#include "Modules/serialize/toplevelserializableunit.h"

#include "Modules/serialize/container/controlledcontainer.h"

namespace Engine {
namespace GUI {

    struct MADGINE_CLIENT_EXPORT TopLevelWindowComponentBase : ScopeBase, MadgineObject, Serialize::SerializableUnitBase {
        TopLevelWindowComponentBase(TopLevelWindow &window);
        virtual ~TopLevelWindowComponentBase() = default;

        TopLevelWindow &window() const;

        virtual const MadgineObject *parent() const override;
        //virtual App::Application &app(bool = true) override;

		TopLevelWindowComponentBase &getSelf(bool = true);

		virtual const char *key() const = 0;

    protected:
        TopLevelWindow &mWindow;
    };

}
}

DECLARE_UNIQUE_COMPONENT(Engine::GUI, TopLevelWindow, MADGINE_CLIENT, TopLevelWindowComponentBase, TopLevelWindow &);

namespace Engine {
namespace GUI {

    class MADGINE_CLIENT_EXPORT TopLevelWindow : public ScopeBase,
                                                 public Input::InputListener,
                                                 public Window::WindowEventListener,
                                                 public Threading::FrameListener,
                                                 public Serialize::TopLevelSerializableUnit<TopLevelWindow> {
        SERIALIZABLEUNIT;

    public:
        TopLevelWindow(GUISystem &gui);
        virtual ~TopLevelWindow();

        void close();

        void swapCurrentRoot(WidgetBase *newRoot);
        void openModalWidget(WidgetBase *widget);
        void closeModalWidget(WidgetBase *widget);
        void openWidget(WidgetBase *widget);
        void closeWidget(WidgetBase *widget);

        /*void showCursor();
			void hideCursor();

			virtual bool isCursorVisible();
			virtual void setCursorVisibility(bool v);
			virtual void setCursorPosition(const Vector2& pos);
			virtual Vector2 getCursorPosition();*/

        bool isHovered(WidgetBase *w);
        WidgetBase *hoveredWidget();

        WidgetBase *getWidget(const std::string &name);

        void registerWidget(WidgetBase *w);

        void unregisterWidget(WidgetBase *w);

        Vector3 getScreenSize();
        std::pair<Vector3, Vector3> getAvailableScreenSpace();

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

        void destroyTopLevel(WidgetBase *w);

        void clear();

        GUISystem &gui();

        //KeyValueMapList maps() override;

        Input::InputHandler *input();

        void addOverlay(WindowOverlay *overlay);

        bool injectKeyPress(const Input::KeyEventArgs &arg) override;
        bool injectKeyRelease(const Input::KeyEventArgs &arg) override;
        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;
        bool injectPointerMove(const Input::PointerEventArgs &arg) override;

        void renderOverlays();

        Window::Window *window() const;
        WidgetBase *currentRoot();

        decltype(auto) widgets()
        {
            return uniquePtrToPtr(mTopLevelWidgets);
        }

        decltype(auto) components()
        {
            return uniquePtrToPtr(mComponents);
        }

        Render::RenderWindow *getRenderer();

        //virtual App::Application &app(bool = true) override;
        //virtual const Core::MadgineObject *parent() const override;

        ToolWindow *createToolWindow(const Window::WindowSettings &settings);
        void destroyToolWindow(ToolWindow *w);

        virtual bool frameStarted(std::chrono::microseconds) override;
        virtual bool frameRenderingQueued(std::chrono::microseconds, Scene::ContextMask) override;
        virtual bool frameEnded(std::chrono::microseconds) override;

        void calculateWindowGeometries();

        template <class T>
        T &getWindowComponent(bool init = true)
        {
            return static_cast<T &>(getWindowComponent(T::component_index(), init));
        }

        TopLevelWindowComponentBase &getWindowComponent(size_t i, bool = true);

    protected:
        void onClose() override;
        void onRepaint() override;
        void onResize(size_t width, size_t height) override;

        WidgetBase *getHoveredWidget(const Vector2 &pos, const Vector3 &screenSize, WidgetBase *current);
        WidgetBase *getHoveredWidgetDown(const Vector2 &pos, const Vector3 &screenSize, WidgetBase *current);

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

    private:
        GUISystem &mGui;

        Window::Window *mWindow = nullptr;
        std::unique_ptr<Render::RenderWindow> mRenderWindow;

        Input::InputHandler *mExternalInput = nullptr;
        std::optional<Input::InputHandlerSelector> mInputHandlerSelector;

        std::map<std::string, WidgetBase *> mWidgets;

        std::vector<std::unique_ptr<ToolWindow>> mToolWindows;

        SERIALIZABLE_CONTAINER(mTopLevelWidgets, std::vector<std::unique_ptr<WidgetBase>>);

        std::vector<WindowOverlay *> mOverlays;



		SERIALIZABLE_CONTAINER_EXT(mComponents, TopLevelWindowContainer<elevate<, Serialize::ControlledContainer, ,std::vector>::type>);

        WidgetBase *mHoveredWidget = nullptr;

        WidgetBase *mCurrentRoot = nullptr;

        std::stack<WidgetBase *> mModalWidgetList;
    };

}
}
