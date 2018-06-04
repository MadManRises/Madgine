#include "maditorviewlib.h"

#include "vssettingswidget.h"
#include "ui_vssettingswidget.h"

#include "VSLink.h"

VSSettingsWidget::VSSettingsWidget(VSLink *link) :    
    ui(new Ui::VSSettingsWidget),
	mVS(link)
{
    ui->setupUi(this);
}

VSSettingsWidget::~VSSettingsWidget()
{
    delete ui;
}

void VSSettingsWidget::setup()
{
	ui->autoAttachBox->setChecked(mVS->autoAttachDebugger());
}

bool VSSettingsWidget::apply()
{
	mVS->setAutoAttachDebugger(ui->autoAttachBox->isChecked());
	return true;
}
