#include "maditorviewlib.h"

#include "editorsettingswidget.h"
#include "ui_editorsettingswidget.h"

#include "model/maditor.h"

namespace Maditor {
namespace View {

EditorSettingsWidget::EditorSettingsWidget(Model::Maditor *model) :
    ui(new Ui::EditorSettingsWidget),
	mModel(model)
{
    ui->setupUi(this);
}

EditorSettingsWidget::~EditorSettingsWidget()
{
    delete ui;
}

void EditorSettingsWidget::setup()
{
	ui->reloadBox->setChecked(mModel->reloadProjectProperty());
}

bool EditorSettingsWidget::apply()
{
	mModel->setReloadProjectProperty(ui->reloadBox->isChecked());
	return true;
}

} // namespace View
} // namespace Maditor
