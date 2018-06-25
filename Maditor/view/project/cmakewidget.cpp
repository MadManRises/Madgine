#include "maditorviewlib.h"

#include "cmakewidget.h"
#include "ui_cmakewidget.h"

#include "../../model/project/cmakeserver.h"

namespace Maditor {
	namespace View {

		CmakeWidget::CmakeWidget(QWidget *parent) :
			QWidget(parent),
			ui(new Ui::CmakeWidget)
		{
			ui->setupUi(this);

			mCursor = ui->log->textCursor();
		}

		CmakeWidget::~CmakeWidget()
		{
			delete ui;
		}

		void CmakeWidget::setModel(Model::CmakeServer* server)
		{
			//connect(server, &Model::CmakeServer::messageReceived, this, &CmakeWidget::output);
		}

		void CmakeWidget::clearModel()
		{
		}

		void CmakeWidget::output(QString msg)
		{
			ui->log->setTextCursor(mCursor);
			msg.replace("\r\n", "\n");
			QStringList list = msg.split('\r');
			bool first = true;
			for (const QString &split : list) {
				if (first)
					first = false;
				else {
					ui->log->moveCursor(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
				}
				if (!split.isEmpty())
					ui->log->insertPlainText(split);
			}
			mCursor = ui->log->textCursor();
			ui->log->ensureCursorVisible();
		}

	} // namespace View
} // namespace Maditor
