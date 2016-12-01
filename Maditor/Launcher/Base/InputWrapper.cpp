#include "madginelib.h"

#include "InputWrapper.h"

#include "Shared.h"

InputWrapper::InputWrapper() :
	mShared(SharedMemory::getSingleton().mInput)
{
	
}

void InputWrapper::update()
{

	while (!mShared.mMouseQueue.empty()) {
		const std::pair<InputEventType, Engine::GUI::MouseEventArgs> &evt = mShared.mMouseQueue.front();
		if (evt.first == PRESS) {
			mSystem->injectMousePress(evt.second);
		}
		else {
			mSystem->injectMouseRelease(evt.second);
		}
		mShared.mMouseQueue.pop();
	}

	while (!mShared.mKeyQueue.empty()) {
		const std::pair<InputEventType, Engine::GUI::KeyEventArgs> &evt = mShared.mKeyQueue.front();
		if (evt.first == PRESS) {
			mSystem->injectKeyPress(evt.second);
		}
		else {
			mSystem->injectKeyRelease(evt.second);
		}
		mShared.mKeyQueue.pop();
	}


	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex);
	if (mShared.mMove) {
		mSystem->injectMouseMove(mShared.mAccumulativeMouseMove);
		mShared.mMove = false;
		mShared.mAccumulativeMouseMove.moveDelta = { 0,0 };
		mShared.mAccumulativeMouseMove.scrollWheel = 0;
	}
}

void InputWrapper::setSystem(Engine::GUI::GUISystem * gui)
{
	mSystem = gui;
}
