#include "maditorlib.h"

#include "Addon.h"

#include <Windows.h>

namespace Maditor {
	namespace Addons {

Addon::Addon()
{

}

Addon::~Addon()
{

}

void Addon::update()
{
}

void Addon::addActions(View::MainWindow * window)
{
}

void Addon::setModel(Model::Editor * editor)
{
}

QString Addon::resourceGroupName()
{
	return QString();
}

void Addon::openFile(const QString & path, int lineNr)
{
}

QStringList Addon::supportedFileExtensions()
{
	return QStringList();
}


AddonCollector::AddonCollector() {

	QFile inputFile("addons.txt");
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			HINSTANCE h = LoadLibrary(line.toStdString().c_str());
			if (!h)
				continue;
			typedef Addon *(*Creator)();
			Creator creator = (Creator)GetProcAddress(h, "createAddon");
			if (!creator)
				continue;
			Addon *addon = creator();

			mAddons.push_back(addon);
		}
		inputFile.close();

		startTimer(500);
	}

}

AddonCollector::~AddonCollector()
{
	for (Addon *addon : mAddons) {
		delete addon;
	}
}

void AddonCollector::timerEvent(QTimerEvent * e)
{
	for (Addon *addon : mAddons) {
		addon->update();
	}
}

std::list<Addon*>::const_iterator AddonCollector::begin()
{
	return mAddons.begin();
}

std::list<Addon*>::const_iterator AddonCollector::end()
{
	return mAddons.end();
}

void AddonCollector::setWindow(View::MainWindow * window)
{
	for (Addon *addon : mAddons) {
		addon->addActions(window);
	}
}

void AddonCollector::setModel(Model::Editor * editor)
{
	for (Addon *addon : mAddons) {
		addon->setModel(editor);
	}
}


	}
}
