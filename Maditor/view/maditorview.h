#pragma once

#include "componentview.h"

#include "model/maditor.h"


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
	
	void setupUi(MainWindow *window);
	virtual void setModel(Model::Maditor *model) override;

	Dialogs::DialogManager *dialogs();

	bool closeEvent();

	void createMainWindow();

private slots:

	void updateRecentProjects(const QStringList &list);
	void recentProjectClicked(QAction *action);

private:

	Dialogs::DialogManager *mDialogManager;

	


	QSettings *mSettings;

	QMenu *mRecentProjectsMenu;
	int mRecentProjectInitialActionCount;
	QAction *mClearRecentProjectsAction;

	MainWindow *mMainWindow;

};


} // namespace View
} // namespace Maditor

