#pragma once

namespace Maditor {
	namespace Model {
		class ApplicationWrapper;
		class Project;
		class ModuleList;
		class Module;

		class Editor;

		class TreeItem;
		class TreeModel;

		class Log;
		class LogsModel;

		class ProjectLog;

		namespace Watcher {
			class ApplicationWatcher;
			
			class PerformanceWatcher;
			class StatsProfilerNode;
			
			class ResourceWatcher;
			class ResourceItem;
			class ResourceGroupItem;
			
			class OgreSceneWatcher;
			class OgreSceneNodeItem;
			
			class OgreLogWatcher;		

			class InputWrapper;

			class ObjectsWatcher;
			
		}

		namespace Generator {
			class ClassGeneratorFactory;
			class ClassGenerator;
			class CmakeSubProject;
		}

		namespace Editors {
			class ScriptEditorModel;
			class VSLink;
			class ScriptEditor;
			class EditorManager;
		}
		
		class ModuleLoader;

	}

	namespace View {
		class OgreWindow;
	}
}