#pragma once

#include <list>

namespace Maditor {
	namespace Addons {

		class MADITOR_MODEL_EXPORT Addon {
		public:
			Addon();
			virtual ~Addon();

			virtual void update();
			virtual void setup(View::MaditorView *view);
			virtual void setupUi(View::MainWindow *window);
			virtual void onProjectOpened(Model::Project *project);

			virtual QString resourceGroupName();
			virtual void openFile(const QString &path, int lineNr = -1);
			virtual QStringList supportedFileExtensions();
		};

		class MADITOR_MODEL_EXPORT AddonCollector : public QObject {
			Q_OBJECT
		public:
			AddonCollector(Model::Maditor *model);
			virtual ~AddonCollector();

			void load(Model::Maditor *editor);
			void unload();

			virtual void timerEvent(QTimerEvent *e) override;

			std::list<Addon*>::const_iterator begin();
			std::list<Addon*>::const_iterator end();

			void setup(View::MaditorView *view);
			void setupUi(View::MainWindow *window);

		public slots:
			void onProjectOpened(Model::Project *project);

		private:
			std::list<Addon*> mAddons;

		};

	}
}