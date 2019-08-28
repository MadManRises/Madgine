#pragma once

#include "../../input/inputcollector.h"
#include "../../input/inputlistener.h"
#include "Interfaces/window/windoweventlistener.h"
#include "Modules/uniquecomponent/uniquecomponentdefine.h"
#include "Modules/uniquecomponent/uniquecomponentselector.h"

#include "Modules/generic/transformIt.h"

#include "Madgine/core/madgineobject.h"

#include "Modules/keyvalue/scopebase.h"

namespace Engine {
namespace GUI {

    struct MADGINE_CLIENT_EXPORT TopLevelWindowComponentBase : ScopeBase, Core::MadgineObject {
        TopLevelWindowComponentBase(TopLevelWindow &window);
        virtual ~TopLevelWindowComponentBase() = default;

        TopLevelWindow &window();

        virtual const MadgineObject *parent() const override;
        virtual App::Application &app(bool = true) override;

    protected:
        TopLevelWindow &mWindow;
    };

}
}

DECLARE_UNIQUE_COMPONENT(Engine::GUI, TopLevelWindow, MADGINE_CLIENT, TopLevelWindowComponentBase, TopLevelWindow &);

namespace Engine {
namespace GUI {

    class MADGINE_CLIENT_EXPORT TopLevelWindow : public ScopeBase, public Input::InputListener, public Window::WindowEventListener, public Threading::FrameListener {

    public:
        TopLevelWindow(GUISystem &gui);
        virtual ~TopLevelWindow();

        void close();

        void swapCurrentRoot(Widget *newRoot);
        void openModalWidget(Widget *widget);
        void closeModalWidget(Widget *widget);
        void openWidget(Widget *widget);
        void closeWidget(Widget *widget);

        /*void showCursor();
			void hideCursor();

			virtual bool isCursorVisible();
			virtual void setCursorVisibility(bool v);
			virtual void setCursorPosition(const Vector2& pos);
			virtual Vector2 getCursorPosition();*/

        bool isHovered(Widget *w);
        Widget *hoveredWidget();

        Widget *getWidget(const std::string &name);

        void registerWidget(Widget *w);

        void unregisterWidget(Widget *w);

        Vector3 getScreenSize();
        std::pair<Vector3, Vector3> getAvailableScreenSpace();

        Matrix3 getSize();

        Widget *createTopLevelWidget(const std::string &name);
        Bar *createTopLevelBar(const std::string &name);
        Button *createTopLevelButton(const std::string &name);
        Checkbox *createTopLevelCheckbox(const std::string &name);
        Combobox *createTopLevelCombobox(const std::string &name);
        Image *createTopLevelImage(const std::string &name);
        Label *createTopLevelLabel(const std::string &name);
        SceneWindow *createTopLevelSceneWindow(const std::string &name);
        TabWidget *createTopLevelTabWidget(const std::string &name);
        Textbox *createTopLevelTextbox(const std::string &name);

        void destroyTopLevel(Widget *w);

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

        Window::Window *window();
        Widget *currentRoot();

        decltype(auto) widgets()
        {
            return uniquePtrToPtr(mTopLevelWidgets);
        }

        decltype(auto) components()
        {
            return uniquePtrToPtr(mComponents);
        }

        Render::RenderWindow *
        getRenderer();

        //virtual App::Application &app(bool = true) override;
        //virtual const Core::MadgineObject *parent() const override;

        UI::UIManager &ui();

        ToolWindow *createToolWindow(const Window::WindowSettings &settings);
        void destroyToolWindow(ToolWindow *w);

        virtual bool frameStarted(std::chrono::microseconds) override;
        virtual bool frameRenderingQueued(std::chrono::microseconds, Scene::ContextMask) override;
        virtual bool frameEnded(std::chrono::microseconds) override;

    protected:
        void onClose() override;
        void onRepaint() override;
        void onResize(size_t width, size_t height) override;

        void calculateWindowGeometries();

        Widget *getHoveredWidget(const Vector2 &pos, const Vector3 &screenSize, Widget *current);
        Widget *getHoveredWidgetDown(const Vector2 &pos, const Vector3 &screenSize, Widget *current);

        std::unique_ptr<Widget> createWidgetClass(const std::string &name, Class _class);

        virtual std::unique_ptr<Widget> createWidget(const std::string &name);
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

        std::map<std::string, Widget *> mWidgets;
        std::unique_ptr<UI::UIManager> mUI;

        std::vector<std::unique_ptr<ToolWindow>> mToolWindows;

        std::vector<std::unique_ptr<Widget>> mTopLevelWidgets;

        std::vector<WindowOverlay *> mOverlays;

        TopLevelWindowContainer<std::vector> mComponents;

        Widget *mHoveredWidget = nullptr;

        Widget *mCurrentRoot = nullptr;

        std::stack<Widget *> mModalWidgetList;
    };

}
}
