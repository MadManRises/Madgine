#pragma once

#include <list>

namespace Maditor {
	namespace Addons {

		class MADITOR_EXPORT Addon {
		public:
			Addon();
			virtual ~Addon();

			virtual void update();
			virtual void addActions(View::MainWindow *window);
			virtual void setModel(Model::Editor *editor);

			virtual QString resourceGroupName();
			virtual void openFile(const QString &path, int lineNr = -1);
			virtual QStringList supportedFileExtensions();
		};

		class MADITOR_EXPORT AddonCollector : public QObject {
			Q_OBJECT
		public:
			AddonCollector(View::MainWindow *window, Model::Editor *editor);
			virtual ~AddonCollector();

			virtual void timerEvent(QTimerEvent *e) override;

			std::list<Addon*>::const_iterator begin();
			std::list<Addon*>::const_iterator end();

			static AddonCollector &getSingleton();
		private:
			std::list<Addon*> mAddons;

			static AddonCollector *sSingleton;
		};

	}
}