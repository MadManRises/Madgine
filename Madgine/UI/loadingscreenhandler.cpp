#include "libinclude.h"
#include "loadingscreenhandler.h"
#include "windownames.h"

#include "Database/messages.h"

#include "UI\UIManager.h"

#include "GUI\Windows\Label.h"
#include "GUI\Windows\Bar.h"


#include "Process.h"

namespace Engine {

	API_IMPL(UI::LoadingScreenHandler, "openLoadingScreen"_(&LoadingScreenHandler::open));

namespace UI {	

	LoadingScreenHandler::LoadingScreenHandler() :
		mCurrentProcess(0),
		GuiHandler("Loading", WindowType::ROOT_WINDOW, "loading")
	{

		registerWindow({ WindowNames::Loading::message, &mLoggingLabel });
		registerWindow({ WindowNames::Loading::progressBar, &mProgressBar });

	}

	LoadingScreenHandler::~LoadingScreenHandler()
	{
		clearProcess();
	}

void LoadingScreenHandler::onSubProcessStarted(const std::string & name)
{
	mLoggingLabel->setText(name);
	mUI->renderFrame();
}

void LoadingScreenHandler::onProgressUpdate(float progress)
{
	mProgressBar->setRatio(progress);
	mUI->renderFrame();
}

void LoadingScreenHandler::setProcess(Process * process)
{
	if (mCurrentProcess)
		mCurrentProcess->removeListener(this);
	mCurrentProcess = process;
	mCurrentProcess->addListener(this);
}

void LoadingScreenHandler::clearProcess()
{
	if (mCurrentProcess)
		mCurrentProcess->removeListener(this);
	mCurrentProcess = 0;
}

void LoadingScreenHandler::open()
{
	GuiHandlerBase::open();
	mUI->renderFrame();
}


}
}
