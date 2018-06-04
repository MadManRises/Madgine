#pragma once


#include "documents/windowspawner.h"

#include "model/application/applicationlauncher.h"

#include "applicationwindow.h"
#include "applicationlog.h"

namespace Maditor {
namespace View {



class ApplicationView : 
	public QObject, 
	public ComponentView<Model::ApplicationLauncher>,
	public WindowSpawner<Model::EmbeddedLauncher, EmbeddedApplicationContainerWindow>,
	public WindowSpawner<Model::StandaloneLauncher, StandaloneApplicationContainerWindow>
{
    Q_OBJECT

public:

	ApplicationView();
	
	void setupUi(MainWindow *window);

	void setConfigModel(Model::ConfigList *list);

protected:
	virtual void setModel(Model::ApplicationLauncher *app) override;
	virtual void clearModel() override;

	virtual void currentTabSet(EmbeddedApplicationContainerWindow *win) override;
	virtual void currentTabSet(StandaloneApplicationContainerWindow *win) override;
	virtual void currentTabCleared(QWidget *w) override;

	void setCurrentTab(QWidget *tab);

	void selectConfig(QAction *action);
	void selectConfigName(const QString &name, bool remote);

private slots:
	void onConfigAdded(Model::ApplicationConfig *config);
	void onInstanceAdded(Model::ApplicationLauncher *instance);
	void onInstanceDestroyed(Model::ApplicationLauncher *instance);
	void createCurrentConfig();

	

private:
	const Ui::MainWindow *mUi;

	Model::ConfigList *mList;

	QMenu *mCurrentConfigSelector;

	QWidget *mCurrentWidget;

	int mApplicationInitialActionCount;

	std::list<QMetaObject::Connection> mAppConnections;

	QAction *mCurrentConfigSeparator;

	static const QString sRemoteSuffix;

};


} // namespace View
} // namespace Maditor

