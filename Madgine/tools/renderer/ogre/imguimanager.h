#pragma once

#include "../../imgui/imgui.h"
#include <OgreCommon.h>

#include <OgreRenderQueueListener.h>
#include <OgreSingleton.h>
#include <OgreTexture.h>

#include "ImguiRenderable.h"
#include "Madgine/input/inputlistener.h"

namespace Ogre
{
    class SceneManager;
    class TextureUnitState;

    class ImguiManager : public RenderQueueListener, public Engine::Input::InputListener
    {
    public:

        ImguiManager(Engine::Input::InputListener *listener);
        ~ImguiManager();

        virtual void init(Ogre::SceneManager* mgr);

        virtual void newFrame(float deltaTime,const Ogre::Rect & windowRect);

        //inherited from RenderQueueListener
        virtual void renderQueueEnded(uint8 queueGroupId, const String& invocation,bool& repeatThisInvocation);

		void injectKeyPress(const Engine::Input::KeyEventArgs& arg) override;
		void injectKeyRelease(const Engine::Input::KeyEventArgs& arg) override;
		void injectMousePress(const Engine::Input::MouseEventArgs& arg) override;
		void injectMouseRelease(const Engine::Input::MouseEventArgs& arg) override;
		void injectMouseMove(const Engine::Input::MouseEventArgs& arg) override;

    protected:

        void createFontTexture();
        void createMaterial();

    protected:
        SceneManager*				mSceneMgr;
        TextureUnitState*           mTexUnit;
        int                         mLastRenderedFrame;

        ImGUIRenderable             mRenderable;
        TexturePtr                  mFontTex;

        bool                        mFrameEnded;

		Engine::Input::InputListener *mListener;
    };
}
