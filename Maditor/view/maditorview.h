#pragma once

#include "componentview.h"

#include "../model/maditor.h"



namespace Maditor {
namespace View {



class MADITOR_VIEW_EXPORT MaditorView : 
	public QObject, 
	public ComponentView<Model::Maditor> 
{
    Q_OBJECT

public:

	MaditorView();
	virtual ~MaditorView();
	
	virtual void setupUi(MainWindow *window) override;
	virtual void setModel(Model::Maditor *model) override;

	Dialogs::DialogManager *dialogs();

	bool closeEvent();

	void createMainWindow();

private slots:

	void updateRecentProjects(const QStringList &list);
	void recentProjectClicked(QAction *action);

	void onProjectOpened(Model::Project *project);
	void onProjectClosed(Model::Project *project);

private:

	std::unique_ptr<Dialogs::DialogManager> mDialogManager;

	


	QSettings mSettings;

	QMenu *mRecentProjectsMenu;
	int mRecentProjectInitialActionCount;
	QAction *mClearRecentProjectsAction;

	MainWindow *mMainWindow;

	ApplicationView *mApplication;

	LogsView *mLogs;

	ProjectView *mProject;

	CmakeView *mCmake;

};


} // namespace View
} // namespace Maditor

