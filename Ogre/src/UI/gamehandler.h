#pragma once

#include "handler.h"

#include "App/contextmasks.h"

#include "Scripting\Types\globalapi.h"

#include "ogreuniquecomponent.h"

namespace Engine {
namespace UI {

class OGREMADGINE_EXPORT GameHandlerBase : public Handler {
public:
    GameHandlerBase(const std::string &windowName, App::ContextMask context = App::ContextMask::SceneContext);

    void abortDrag();

	void update(float timeSinceLastFrame, App::ContextMask mask);
    void fixedUpdate(float timeStep, App::ContextMask mask);

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

	virtual void update(float timeSinceLastFrame);
	virtual void fixedUpdate(float timeStep);




    void clampToWindow(GUI::MouseEventArgs &me);

    enum class MouseDragMode{
        DISABLED = 0,
        ENABLED,
        ENABLED_HIDECURSOR
    };

    void setMouseDragMode(GUI::MouseButton::MouseButton button, MouseDragMode mode);

    const std::array<float, 2> &dragStart();

private:
    static const float mDragStartThreshold;

    std::array<MouseDragMode, GUI::MouseButton::BUTTON_COUNT> mMouseDragModes;

	GUI::MouseButton::MouseButton mCurrentMouseButton;

    bool mDragging, mSingleClick;

    std::array<float, 2> mDragStart;    

    App::ContextMask mContext;

	Scene::OgreSceneManager *mSceneMgr;
};

template <class T>
class GameHandler : public OgreUniqueComponent<T, GameHandlerBase>, public Scripting::GlobalAPI<T>, public Hierarchy::HierarchyObject<T> {
	using OgreUniqueComponent<T, GameHandlerBase>::OgreUniqueComponent;
};


} // namespace UI
}


