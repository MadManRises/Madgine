#include "maditorviewlib.h"

#include "documentview.h"
#include "model/documents/document.h"

namespace Maditor {
	namespace View {
		DocumentView::DocumentView(Model::Document * doc, QWidget *widget) :
			mDocument(doc),
			mWidget(widget),
			mSaveShortcut(QKeySequence(QObject::tr("Ctrl+S", "Save")), widget)
		{
			mConnection = QObject::connect(doc, &Model::Document::dirtyFlagChanged, [=](bool flag) {updateTitle(); });
			QObject::connect(&mSaveShortcut, &QShortcut::activated, doc, &Model::Document::save);
			updateTitle();
		}
		DocumentView::~DocumentView()
		{
			QObject::disconnect(mConnection);
		}
		Model::Document * DocumentView::document()
		{
			return mDocument;
		}
		void DocumentView::updateTitle()
		{
			mWidget->setWindowTitle(mDocument->getQualifiedName());
		}
		QMessageBox::StandardButton DocumentView::requestClose(bool multipleFiles)
		{
			QMessageBox::StandardButton answer = QMessageBox::NoButton;
			if (mDocument->getDirtyFlag()) {
				answer = 
					QMessageBox::question(NULL, "Unsaved Changes", QString("There are unsaved changes in <i>%1</i>! Save?").arg(mDocument->getName()), 
						QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort | (multipleFiles ? QMessageBox::YesToAll | QMessageBox::NoToAll : QMessageBox::NoButton));
				switch (answer) {
				case QMessageBox::YesToAll:
				case QMessageBox::Yes:
					mDocument->save();
					break;
				case QMessageBox::No:
				case QMessageBox::NoToAll:
					mDocument->discardChanges();
					break;
				default:
					break;
				}
			}
			return answer;
		}
		void DocumentView::save()
		{
			if (mDocument->getDirtyFlag()) {
				mDocument->save();
			}
		}
		void DocumentView::discardChanges()
		{
			if (mDocument->getDirtyFlag()) {
				mDocument->discardChanges();
			}
		}
	}
}
