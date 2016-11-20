#pragma once

struct InputShared;

class InputWrapper : public Engine::Input::InputHandler {

public:
	InputWrapper();

				
	virtual void update() override;

	void setSystem(Engine::GUI::GUISystem *gui);

				
private:
	Engine::GUI::GUISystem *mSystem;

	InputShared &mShared;

};
			