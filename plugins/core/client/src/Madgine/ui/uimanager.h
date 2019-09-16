#pragma once

#include "Madgine/threading/framelistener.h"
#include "Modules/uniquecomponent/uniquecomponentcontainer.h"
#include "gamehandler.h"
#include "guihandler.h"

#include "Modules/keyvalue/scopebase.h"

#include "../gui/widgets/toplevelwindow.h"

namespace Engine {
namespace UI {
    struct MADGINE_CLIENT_EXPORT UIManager : GUI::TopLevelWindowComponent<UIManager>,
                                             Threading::FrameListener {
        UIManager(GUI::TopLevelWindow &window);
        ~UIManager();

        void clear();

        void hideCursor(bool keep = true);
        void showCursor();
        bool isCursorVisible() const;

        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;
        bool frameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

        //Scene::ContextMask currentContext();

        GUI::TopLevelWindow &window(bool = true) const;

        std::set<GameHandlerBase *> getGameHandlers();
        std::set<GuiHandlerBase *> getGuiHandlers();

        static const constexpr int sMaxInitOrder = 4;

        const char *key() const;

        Scene::SceneComponentBase &getSceneComponent(size_t i, bool = true);

        App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

        template <class T>
        T &getGuiHandler(bool init = true)
        {
            return static_cast<T &>(getGuiHandler(T::component_index(), init));
        }

        GuiHandlerBase &getGuiHandler(size_t i, bool = true);

        template <class T>
        T &getGameHandler(bool init = true)
        {
            return static_cast<T &>(getGameHandler(T::component_index(), init));
        }

        GameHandlerBase &getGameHandler(size_t i, bool = true);

        Scene::SceneManager &sceneMgr(bool = true);

        UIManager &getSelf(bool = true);

        //virtual App::Application &app(bool = true) override;
        virtual const MadgineObject *parent() const override;

    protected:
        bool init() override;
        void finalize() override;

        //KeyValueMapList maps() override;

    private:

        Vector2 mKeptCursorPosition;
        bool mKeepingCursorPos = false;

        GuiHandlerContainer<std::vector> mGuiHandlers;
        GameHandlerContainer<std::vector> mGameHandlers;
    };
}
}