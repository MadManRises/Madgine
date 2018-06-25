#include "maditorviewlib.h"

#include "mainwindow.h"
#include "mainwindow_ui.h"

#include "applicationview.h"

#include "../../model/project/configlist.h"

#include "applicationwindow.h"


namespace Maditor {
namespace View {

	const QString ApplicationView::sRemoteSuffix = " (remote)";

	ApplicationView::ApplicationView() :
		mList(nullptr), mCurrentConfigSelector(nullptr),
		mCurrentWidget(nullptr), mApplicationInitialActionCount(0), mCurrentConfigSeparator(nullptr)
	{
	}

	void ApplicationView::setupUi(MainWindow * window)
	{
		ComponentView::setupUi(window);

		WindowSpawner<Model::StandaloneLauncher, StandaloneApplicationContainerWindow>::setupUi(ui());
		WindowSpawner<Model::EmbeddedLauncher, EmbeddedApplicationContainerWindow>::setupUi(ui());


		mApplicationInitialActionCount = ui()->menuDocument->actions().count();

		mCurrentConfigSelector = new QMenu(window);
		mCurrentConfigSelector->setEnabled(false);

		connect(mCurrentConfigSelector, &QMenu::triggered, this, &ApplicationView::selectConfig);
		connect(mCurrentConfigSelector->menuAction(), &QAction::triggered, this, &ApplicationView::createCurrentConfig);


		QToolBar *toolbar = new QToolBar(window);
		toolbar->setObjectName("ConfigSelector");

		toolbar->addAction(mCurrentConfigSelector->menuAction());

		window->addToolBar(toolbar);

	}

	void ApplicationView::setConfigModel(Model::ConfigList * list)
	{
		mList = list;

		mCurrentConfigSelector->clear();
		mCurrentConfigSeparator = mCurrentConfigSelector->addSeparator();
		for (const QString &action : list->getConfigs()) {
			mCurrentConfigSelector->insertAction(mCurrentConfigSeparator, new QAction(action));
			mCurrentConfigSelector->addAction(action + sRemoteSuffix);
		}		

		if (mCurrentConfigSelector->actions().size() > 1) {
			mCurrentConfigSelector->setEnabled(true);
			mCurrentConfigSelector->setTitle(QString("New %1").arg(mCurrentConfigSelector->actions().front()->text()));
		}
		else {
			mCurrentConfigSelector->setEnabled(false);
		}

		connect(list, &Model::ConfigList::instanceAdded, this, &ApplicationView::onInstanceAdded);
		connect(list, &Model::ConfigList::instanceDestroyed, this, &ApplicationView::onInstanceDestroyed);
	}

	void ApplicationView::clearConfigModel()
	{
		mList = nullptr;

		mCurrentConfigSelector->clear();

	}

	void ApplicationView::setModel(Model::ApplicationLauncher * app)
	{
		
		ui()->modulesWidget->setModel(app->moduleLoader());
		ui()->performanceWidget->setModel(app->util()->profiler());
		ui()->appStatsWidget->setModel(app->util()->stats());
		ui()->inspectorWidget->setModel(app->inspector());
	}

	void ApplicationView::clearModel()
	{	
		ui()->modulesWidget->setModel(nullptr);
		ui()->appStatsWidget->clearModel();
	}

	void ApplicationView::currentTabSet(StandaloneApplicationContainerWindow * win)
	{
		setModel(win->app());
		setCurrentTab(win);
	}
	
	void ApplicationView::currentTabSet(EmbeddedApplicationContainerWindow * win)
	{
		setModel(win->app());
		setCurrentTab(win);
	}
	
	void ApplicationView::currentTabCleared(QWidget * w)
	{
		if (mCurrentWidget != w) {
			clearModel();
			ui()->menuDocument->menuAction()->setVisible(false);
		}
	}

	void ApplicationView::setCurrentTab(QWidget * tab)
	{
		mCurrentWidget = tab;

		QMenu *menu = ui()->menuDocument;
		menu->setTitle("Application");
		for (QAction *action : menu->actions().mid(mApplicationInitialActionCount)) {
			menu->removeAction(action);
		}

		for (QAction *a : tab->actions()) {
			menu->addAction(a);
		}

		menu->menuAction()->setVisible(true);

	}

	void ApplicationView::selectConfig(QAction *action)
	{
		QString text = action->text();

		if (text.endsWith(sRemoteSuffix))
		{
			selectConfigName(action->text().mid(0, text.size() - sRemoteSuffix.size()), true);
		}else
		{
			selectConfigName(text, false);	
		}		
	}

	void ApplicationView::selectConfigName(const QString &name, bool remote)
	{
		Model::ApplicationConfig *config = mList->getConfig(name);
		assert(config);
		std::shared_ptr<Model::ApplicationLauncher> launcher = config->createInstace(remote);
		//setModel(launcher);
	}

	void ApplicationView::onConfigAdded(Model::ApplicationConfig *config) {
		mCurrentConfigSelector->addAction(config->name());
		if (!mCurrentConfigSelector->isEnabled()) {
			mCurrentConfigSelector->setEnabled(true);
			mCurrentConfigSelector->setTitle(QString("New %1").arg(config->name()));
		}
	}

	void ApplicationView::onInstanceAdded(const std::shared_ptr<Model::ApplicationLauncher> &instance)
	{
		if (instance->isRemote()){
			WindowSpawner<Model::StandaloneLauncher, StandaloneApplicationContainerWindow>::spawn(dynamic_cast<Model::StandaloneLauncher*>(instance.get()));
		}else{
			WindowSpawner<Model::EmbeddedLauncher, EmbeddedApplicationContainerWindow>::spawn(dynamic_cast<Model::EmbeddedLauncher*>(instance.get()));
		}
	}

	void ApplicationView::onInstanceDestroyed(const std::shared_ptr<Model::ApplicationLauncher> &instance)
	{
		if (instance->isRemote()){
			WindowSpawner<Model::StandaloneLauncher, StandaloneApplicationContainerWindow>::remove(dynamic_cast<Model::StandaloneLauncher*>(instance.get()));
		}else{
			WindowSpawner<Model::EmbeddedLauncher, EmbeddedApplicationContainerWindow>::remove(dynamic_cast<Model::EmbeddedLauncher*>(instance.get()));
		}
	}

	void ApplicationView::createCurrentConfig()
	{
		selectConfigName(mCurrentConfigSelector->title().mid(4), false);
	}


} // namespace View
} // namespace Maditor
