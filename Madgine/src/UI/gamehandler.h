#pragma once

#include "handler.h"

#include "uniquecomponent.h"
#include "Scene/scenelistener.h"

#include "App/contextmasks.h"

#include "Scripting\Types\globalapi.h"

namespace Engine {
namespace UI {

class MADGINE_EXPORT GameHandlerBase : public Handler, public Scene::SceneListener {
public:
    GameHandlerBase(const std::string &windowName, App::ContextMask context = App::ContextMask::SceneContext);

    void abortDrag();

    void update(float timeSinceLastFrame, App::ContextMask mask);

    virtual void update(float timeSinceLastFrame);


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
};

template <class T>
class GameHandler : public UniqueComponent<T, GameHandlerBase>, public Scripting::GlobalAPI<T> {
	using UniqueComponent<T, GameHandlerBase>::UniqueComponent;
};


} // namespace UI
}


