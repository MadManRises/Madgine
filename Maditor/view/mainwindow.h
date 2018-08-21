#ifndef MADITOR_VIEW_MAINWINDOW_H
#define MADITOR_VIEW_MAINWINDOW_H

#include <QMainWindow>
#include "componentview.h"

#include "../model/maditor.h"
#include "application/inspector/inspectorwidget.h"

#include "application/inspector/inspectorlayoutdata.h"

namespace Maditor {
namespace View {

namespace Ui {
class MainWindow;
}

class MADITOR_VIEW_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

	virtual void closeEvent(QCloseEvent * event) override;

	//Dialogs::DialogManager *dialogs();

	const Ui::MainWindow * ui();

	InspectorWidget * inspector();


private:    
	Ui::MainWindow *const mUi;

	InspectorWidget * mInspector;

	InspectorLayoutData mInspectorData;

};


} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_MAINWINDOW_H
