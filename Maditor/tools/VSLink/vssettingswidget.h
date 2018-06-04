#ifndef VSSETTINGSWIDGET_H
#define VSSETTINGSWIDGET_H

#include "View\Dialogs\settingstab.h"

namespace Ui {
class VSSettingsWidget;
}

class VSLink;

class VSSettingsWidget : public Maditor::View::Dialogs::SettingsTab
{
    Q_OBJECT

public:
    explicit VSSettingsWidget(VSLink *link);
    ~VSSettingsWidget();

	virtual void setup() override;
	virtual bool apply() override;

private:
    Ui::VSSettingsWidget *ui;

	VSLink *mVS;
};

#endif // VSSETTINGSWIDGET_H
