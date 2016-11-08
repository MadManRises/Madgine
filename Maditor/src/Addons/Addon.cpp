#include "madgineinclude.h"

#include "Addon.h"

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


AddonCollector::AddonCollector(View::MainWindow *window, Model::Editor *editor) {

	sSingleton = this;

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
			Creator creator = (Creator)GetProcAddress(h, "create");
			if (!creator)
				continue;
			Addon *addon = creator();
			addon->addActions(window);
			addon->setModel(editor);
			mAddons.push_back(addon);
		}
		inputFile.close();
	}
	else {
		throw 0;
	}

	startTimer(500);
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

AddonCollector *AddonCollector::sSingleton = 0;

AddonCollector & AddonCollector::getSingleton()
{
	assert(sSingleton);
	return *sSingleton;
}


	}
}
