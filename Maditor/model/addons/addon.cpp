#include "maditormodellib.h"

#include "addon.h"

#ifdef _WIN32
#include <Windows.h>
#undef NO_ERROR
#elif __linux__
#include <dlfcn.h>
#endif

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

void Addon::setup(View::MaditorView *view)
{
}

void Addon::setupUi(View::MainWindow * window)
{
}

void Addon::onProjectOpened(Model::Project * project)
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


AddonCollector::AddonCollector(Model::Maditor *model) {
	load(model);	
}

AddonCollector::~AddonCollector()
{
	for (Addon *addon : mAddons) {
		delete addon;
	}
}

void AddonCollector::load(Model::Maditor * editor)
{
	QFile inputFile("addons.txt");
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			qDebug() << "Loading Addon: " << line;
#ifdef _WIN32
			HINSTANCE h = LoadLibrary(line.toStdString().c_str());
#elif __linux__
			void *h = dlopen(line.toStdString().c_str(), RTLD_LAZY);
#endif
			if (!h) {
				qDebug() << "Failed!";
				continue;
			}
			qDebug() << "Success!";
			typedef Addon *(*Creator)(Model::Maditor*);
#ifdef _WIN32
			Creator creator = (Creator)GetProcAddress(h, "createAddon");
#elif __linux__
			Creator creator = (Creator)dlsym(h, "createAddon");
#endif
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

void AddonCollector::setup(View::MaditorView * view)
{
	for (Addon *addon : mAddons) {
		addon->setup(view);
	}
}

void AddonCollector::setupUi(View::MainWindow * window)
{
	for (Addon *addon : mAddons) {
		addon->setupUi(window);
	}
}

void AddonCollector::onProjectOpened(Model::Project *project) {
	for (Addon *addon : mAddons) {
		addon->onProjectOpened(project);
	}
}


	}
}
