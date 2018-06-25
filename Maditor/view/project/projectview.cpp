#include "maditorviewlib.h"

#include "mainwindow.h"
#include "mainwindow_ui.h"

#include "projectview.h"


#include "../../model/project/configlist.h"


namespace Maditor {
namespace View {

	ProjectView::ProjectView(){

	}

	void ProjectView::setupUi(MainWindow * window)
	{
		ComponentView::setupUi(window);

		IndependentWindowSpawner<Model::ApplicationConfig, ConfigWidget>::setupUi(ui());
		IndependentWindowSpawner<Model::Project, PropertiesWidget>::setupUi(ui());


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


		ui()->projectWidget->setModel(model()->model());
		ui()->mediaWidget->setModel(model()->getMedia());

		connect(project, &Model::Project::showProperties, std::bind(&IndependentWindowSpawner<Model::Project, PropertiesWidget>::spawn<>, this, project));
		connect(project->configList(), &Model::ConfigList::configAdded, this, &ProjectView::openConfig);
		connect(project->configList(), &Model::ConfigList::openConfig, this, &ProjectView::openConfig);
		connect(project->configList(), &Model::ConfigList::configRemoved, this, &ProjectView::closeConfig);
	}

	void ProjectView::clearModel()
	{
		ui()->projectWidget->clearModel();
		ui()->mediaWidget->clearModel();

		ComponentView::clearModel();
	}

	void ProjectView::openConfig(Model::ApplicationConfig * config)
	{
		IndependentWindowSpawner<Model::ApplicationConfig, ConfigWidget>::spawn(config);
	}

	void ProjectView::closeConfig(Model::ApplicationConfig * config)
	{
		IndependentWindowSpawner<Model::ApplicationConfig, ConfigWidget>::remove(config);
	}

} // namespace View
} // namespace Maditor
