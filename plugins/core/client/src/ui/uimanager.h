#pragma once

#include "Modules/threading/framelistener.h"
#include "Modules/uniquecomponent/uniquecomponentcontainer.h"
#include "gamehandler.h"
#include "guihandler.h"

#include "Modules/keyvalue/scopebase.h"

namespace Engine {
namespace UI {
    class MADGINE_CLIENT_EXPORT UIManager : public ScopeBase,
                                            public Core::MadgineObject,
                                            public Threading::FrameListener {
    public:
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

        virtual App::Application &app(bool = true) override;
        virtual const Core::MadgineObject *parent() const override;

    protected:
        bool init() override;
        void finalize() override;

        //KeyValueMapList maps() override;

    private:
        GUI::TopLevelWindow &mWindow;        
			
		GuiHandlerContainer<std::vector> mGuiHandlers;
        GameHandlerContainer<std::vector> mGameHandlers;

    private:
        Vector2 mKeptCursorPosition;
        bool mKeepingCursorPos;
    };
}
}