#include "maditorviewlib.h"

#include "mainwindow.h"
#include "mainwindow_ui.h"

#include "projectview.h"


#include "model/project/configlist.h"


namespace Maditor {
namespace View {

	ProjectView::ProjectView() :
		mUi(nullptr){

	}

	void ProjectView::setupUi(MainWindow * window)
	{
		mUi = window->ui;

		IndependentWindowSpawner<Model::ApplicationConfig, ConfigWidget>::setupUi(mUi);
		IndependentWindowSpawner<Model::Project, PropertiesWidget>::setupUi(mUi);


		/*createToolbar(window, "ApplicationToolbar", {
			{ ui->actionInit, &Model::Project::init },
			{ ui->actionInit_No_Debug, &Model::ApplicationLauncher::initNoDebug },
			{ ui->actionFinalize, &Model::ApplicationLauncher::finalize },
			{ ui->actionStart, &Model::ApplicationLauncher::start },
			{ ui->actionPause, &Model::ApplicationLauncher::pause },
			{ ui->actionStop, &Model::ApplicationLauncher::stop },
		})*/;

	}

	void ProjectView::setModel(Model::Project * project)
	{
		ComponentView::setModel(project);


		mUi->projectWidget->setModel(model()->model());
		mUi->mediaWidget->setModel(model()->getMedia());

		connect(project, &Model::Project::showProperties, std::bind(&IndependentWindowSpawner<Model::Project, PropertiesWidget>::spawn<>, this, project));
		connect(project->configList(), &Model::ConfigList::openConfig, this, &ProjectView::openConfig);
	}

	void ProjectView::openConfig(Model::ApplicationConfig * config)
	{
		IndependentWindowSpawner<Model::ApplicationConfig, ConfigWidget>::spawn(config);
	}

} // namespace View
} // namespace Maditor
