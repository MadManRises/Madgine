#include <maditorviewlib.h>

#include "VSLinkView.h"

#include "VSLink.h"

#include "vssettingswidget.h"

#include "View/mainwindow.h"
#include "View/maditorview.h"

VSLinkView::VSLinkView(VSLink *model) :
	mOpenAction(nullptr){
	setModel(model);

	mOpenAction = new QAction("Open Solution");

	setConnections({
		{ mOpenAction, &VSLink::openSolution }
	});

	createMenu("VSLink", {
		mOpenAction
	});

}

void VSLinkView::setup(Maditor::View::MaditorView *view) {
	

	createSettingsTab(view->dialogs(), new VSSettingsWidget(model()), "Visual Studio");
}

void VSLinkView::setupUi(Maditor::View::MainWindow * window)
{
	addItemsToWindow(window);
}


