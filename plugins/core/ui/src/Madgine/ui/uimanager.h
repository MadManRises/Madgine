#pragma once

#include "Madgine/threading/framelistener.h"
#include "gamehandler.h"
#include "guihandler.h"

#include "Modules/keyvalue/scopebase.h"

#include "Madgine/gui/toplevelwindowcomponentcollector.h"

#include "Modules/madgineobject/madgineobjectobserver.h"

#include "Modules/serialize/container/controlledcontainer.h"

namespace Engine {
namespace UI {
    struct MADGINE_UI_EXPORT UIManager : GUI::TopLevelWindowComponent<UIManager>,
                                         Threading::FrameListener {
        SERIALIZABLEUNIT;

        UIManager(GUI::TopLevelWindow &window);
        ~UIManager();

        void clear();

        void hideCursor(bool keep = true);
        void showCursor();
        bool isCursorVisible() const;

        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;
        bool frameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;

        //Scene::ContextMask currentContext();

        GUI::TopLevelWindow &window(bool = true) const;

        std::set<GameHandlerBase *> getGameHandlers();
        std::set<GuiHandlerBase *> getGuiHandlers();

        static const constexpr int sMaxInitOrder = 4;

        const char *key() const override;

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

        UIManager &getSelf(bool = true);

        OFFSET_CONTAINER(mGuiHandlers, GuiHandlerContainer<Serialize::ControlledContainer<std::vector<Placeholder<0>>, MadgineObjectObserver>>);
        OFFSET_CONTAINER(mGameHandlers, GameHandlerContainer<Serialize::ControlledContainer<std::vector<Placeholder<0>>, MadgineObjectObserver>>)

    protected:
        bool init() override;
        void finalize() override;

    private:
        Vector2 mKeptCursorPosition;
        bool mKeepingCursorPos = false;
    };
}
}