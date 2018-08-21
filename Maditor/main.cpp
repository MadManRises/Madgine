#include "view/maditorviewlib.h"

#include "view/maditorview.h"

#include "model/maditor.h"

#include <QApplication>
#include <experimental/filesystem>

#include <iostream>

int main(int argc, char **argv) 
{

	QApplication app(argc, argv);

	qRegisterMetaType<Engine::ValueType>();

	Maditor::Model::Maditor model;

	Maditor::View::MaditorView view;
	view.createMainWindow();
	
	view.setModel(&model);	

	model.init();

	return app.exec();
}
