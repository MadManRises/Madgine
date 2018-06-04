#ifndef MADITOR_VIEW_MODULEPROPERTIESWIDGET_H
#define MADITOR_VIEW_MODULEPROPERTIESWIDGET_H


namespace Maditor {
namespace View {

namespace Ui {
class ModulePropertiesWidget;
}

class ModulePropertiesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModulePropertiesWidget(Model::Module *module);
    ~ModulePropertiesWidget();

protected:
	void update(QListWidgetItem *item);

private:
    Ui::ModulePropertiesWidget *ui;

	Model::Module *mModule;
};


} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_MODULEPROPERTIESWIDGET_H
