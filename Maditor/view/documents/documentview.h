#pragma once


namespace Maditor {
	namespace View {

class DocumentView
{
public:
	DocumentView(Model::Document *doc, QWidget *widget);
	virtual ~DocumentView();	

	Model::Document *document();

	void updateTitle();

	virtual QMessageBox::StandardButton requestClose(bool multipleFiles);
	void save();
	void discardChanges();

private:
	Model::Document *mDocument;
	QWidget *mWidget;
	QMetaObject::Connection mConnection;

	QShortcut mSaveShortcut;
	
};

	}
}

