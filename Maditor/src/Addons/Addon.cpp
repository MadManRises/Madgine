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

	
}

AddonCollector::~AddonCollector()
{
	for (Addon *addon : mAddons) {
		delete addon;
	}
}

void AddonCollector::load(Model::Editor * editor)
{
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
			typedef Addon *(*Creator)(Model::Editor*);
			Creator creator = (Creator)GetProcAddress(h, "createAddon");
			if (!creator)
				continue;
			Addon *addon = creator(editor);

			mAddons.push_back(addon);
		}
		inputFile.close();

		startTimer(500);
	}

}

void AddonCollector::unload()
{
	for (Addon *addon : mAddons) {
		delete addon;
	}
	mAddons.clear();
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


	}
}
