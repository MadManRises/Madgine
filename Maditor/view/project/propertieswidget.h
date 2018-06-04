#ifndef MODULEPROPERTIESDIALOG_H
#define MODULEPROPERTIESDIALOG_H

#include "documents/documentview.h"

namespace Maditor {
	namespace View {

			namespace Ui {
				class PropertiesWidget;
			}

class PropertiesWidget : public QWidget, public DocumentView
{
    Q_OBJECT

public:
    explicit PropertiesWidget(Model::Project *list);
    ~PropertiesWidget();


private:
	Model::Project *mProject;
	std::list<ModulePropertiesWidget*> mModuleWidgets;

    Ui::PropertiesWidget *ui;
};


	}
}

#endif // MODULEPROPERTIESDIALOG_H
