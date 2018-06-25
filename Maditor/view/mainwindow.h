#ifndef MADITOR_VIEW_MAINWINDOW_H
#define MADITOR_VIEW_MAINWINDOW_H

#include <QMainWindow>
#include "componentview.h"

#include "../model/maditor.h"

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

	const Ui::MainWindow *const ui;


private:    

};


} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_MAINWINDOW_H
