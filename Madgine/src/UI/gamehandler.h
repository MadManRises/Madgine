#pragma once

#include "handler.h"

#include "ogreuniquecomponent.h"

#include "App/contextmasks.h"

#include "Scripting\Types\globalapi.h"

namespace Engine {
namespace UI {

class MADGINE_EXPORT GameHandlerBase : public Handler {
public:
    GameHandlerBase(const std::string &windowName, App::ContextMask context = App::ContextMask::SceneContext);

    void abortDrag();

    void update(float timeSinceLastFrame, App::ContextMask mask);

    virtual void update(float timeSinceLastFrame);

	Scene::OgreSceneManager *sceneMgr();

protected:
    virtual void onMouseMove(GUI::MouseEventArgs &me);

    virtual void onMouseDown(GUI::MouseEventArgs &me);

    virtual void onMouseUp(GUI::MouseEventArgs &me);

    virtual void onMouseHover(const GUI::MouseEventArgs &evt);

    virtual void onMouseClick(const GUI::MouseEventArgs &evt);

    virtual void onMouseDragBegin(const GUI::MouseEventArgs &evt);

    virtual void onMouseDrag(const GUI::MouseEventArgs &evt);

    virtual void onMouseDragEnd(const GUI::MouseEventArgs &evt);

    virtual void onMouseDragAbort();




    void clampToWindow(GUI::MouseEventArgs &me);

    enum class MouseDragMode{
        DISABLED = 0,
        ENABLED,
        ENABLED_HIDECURSOR
    };

    void setMouseDragMode(GUI::MouseButton::MouseButton button, MouseDragMode mode);

    const Ogre::Vector2 &dragStart();

private:
    static const float mDragStartSquaredThreshold;

    std::array<MouseDragMode, GUI::MouseButton::BUTTON_COUNT> mMouseDragModes;

	GUI::MouseButton::MouseButton mCurrentMouseButton;

    bool mDragging, mSingleClick;

    Ogre::Vector2 mDragStart;    

    App::ContextMask mContext;

	Scene::OgreSceneManager *mSceneMgr;
};

template <class T>
class GameHandler : public OgreUniqueComponent<T, GameHandlerBase>, public Scripting::GlobalAPI<T> {
	using OgreUniqueComponent<T, GameHandlerBase>::OgreUniqueComponent;

	virtual const char *getName() override {
		return typeid(T).name();
	}
};


} // namespace UI
}


