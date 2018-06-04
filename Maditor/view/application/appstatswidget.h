#ifndef MADITOR_VIEW_APPSTATSWIDGET_H
#define MADITOR_VIEW_APPSTATSWIDGET_H

namespace Maditor {
namespace View {

namespace Ui {
class AppStatsWidget;
}

class AppStatsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AppStatsWidget(QWidget *parent = 0);
    ~AppStatsWidget();

	void setModel(Model::StatsModel *model);
	void clearModel();

	void setFPS(float fps);
	void setMemUsage(int mem);
	void setOgreMem(int mem);

private:
    Ui::AppStatsWidget *ui;

	std::list<QMetaObject::Connection> mCurrentConnections;
};


} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_APPSTATSWIDGET_H
