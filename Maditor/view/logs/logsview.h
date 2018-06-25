#pragma once

#include "componentview.h"

#include "../../model/logs/logtablemodel.h"

namespace Maditor {
namespace View {



class LogsView : public QObject, public ComponentView<Model::LogTableModel>
{
    Q_OBJECT

public:

	LogsView();
	
	virtual void setupUi(MainWindow *window) override;
	virtual void setModel(Model::LogTableModel *logs) override;

private:
	LogWidget *mLogsWidget;

};


} // namespace View
} // namespace Maditor

