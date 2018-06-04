#pragma once



namespace Maditor {
	namespace Model {

		class Document;
		class DocumentStore;

		class Maditor;
		class Project;
		class ApplicationConfig;
		class ApplicationLauncher;
		class StandaloneLauncher;
		class EmbeddedLauncher;
		class OgreWindow;

		class Module;
		class ModuleList;

		class ConfigList;

		class ModuleLoader;

		class InputWrapper;

		class TreeItem;
		class TreeUnitBase;
		class TreeModel;

		class ProfilerModel;
		class StatsModel;

		class LogsModel;
		class Log;
		class LogTableModel;

		class DialogManager;

		class Inspector;
		class ScopeWrapperItem;

		namespace Generators {
			class CmakeProject;
			class CmakeGenerator;
			class ClassGenerator;
			class ServerClassGenerator;
		}

	}

	namespace View {
		class MainWindow;
		class MaditorView;
		namespace Dialogs {
			class SettingsDialog;
		}
		namespace Ui {
			class MainWindow;
		}
	}

	namespace Addons {
		class AddonCollector;
	}

}