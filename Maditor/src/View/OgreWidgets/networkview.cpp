#include "networkview.h"
#include "ui_networkview.h"

#include "Model\Network\MadgineNetworkClient.h"

namespace Maditor {
	namespace View {

		NetworkView::NetworkView(QWidget *parent) :
			QWidget(parent),
			ui(new Ui::NetworkView)
		{
			ui->setupUi(this);
		}

		NetworkView::~NetworkView()
		{
			delete ui;
		}

		void NetworkView::setModel(Model::Network::MadgineNetworkClient * model)
		{
			connect(ui->connectButton, &QPushButton::clicked, model, &Model::Network::MadgineNetworkClient::connectToHost);
			connect(model, &Model::Network::MadgineNetworkClient::receivedValue, this, &NetworkView::addValue);
		}

		void NetworkView::addValue(const QVariant &val)
		{
			ui->textBrowser->append(val.toString());
		}

	}
}