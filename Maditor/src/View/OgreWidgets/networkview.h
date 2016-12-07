#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include <QWidget>

namespace Maditor {
	namespace View {

		namespace Ui {
			class NetworkView;
		}

		class NetworkView : public QWidget
		{
			Q_OBJECT

		public:
			explicit NetworkView(QWidget *parent = 0);
			~NetworkView();

			void setModel(Model::Network::MadgineNetworkClient *model);

		public slots:
			void addValue(const QVariant &val);

		private:
			Ui::NetworkView *ui;
		};

	}
}

#endif // NETWORKVIEW_H
