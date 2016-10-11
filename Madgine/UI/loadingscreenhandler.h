#pragma once

#include "UI/guihandler.h"
#include "ProcessListener.h"

namespace Engine {
namespace UI {

class MADGINE_EXPORT LoadingScreenHandler : public GuiHandler<LoadingScreenHandler>, public ProcessListener {
public:
    LoadingScreenHandler();
	~LoadingScreenHandler();

	// Inherited via ProcessListener
	virtual void onSubProcessStarted(const std::string & name) override;

	virtual void onProgressUpdate(float progress) override;

	void setProcess(Process *process);
	void clearProcess();

	virtual void open() override;

private:


    GUI::Bar *mProgressBar;
    GUI::Label *mLoggingLabel;

	Process *mCurrentProcess;

};

}
}



