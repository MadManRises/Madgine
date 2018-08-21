#pragma once

namespace Maditor
{
	namespace View
	{

		class InspectorLayout
		{
		public:
			InspectorLayout(InspectorLayoutData *data, const QString &path);

			void draw(Model::ScopeWrapperItem *scope, std::set<std::string> &drawn);

			QStringList associations();
			
		protected:
			void drawSingleElement(QDomElement element, Model::ScopeWrapperItem *scope, std::set<std::string> &drawn);
			void drawElementList(QDomElement element, Model::ScopeWrapperItem *scope, std::set<std::string> &drawn);
			void inheritLayout(QDomElement element, Model::ScopeWrapperItem *scope, std::set<std::string> &drawn);
			void drawConstantString(QDomElement element, Model::ScopeWrapperItem *scope, std::set<std::string> &drawn);
			

		private:
			QString mPath;

			QDomDocument mDocument;

			static std::map<QString, void (InspectorLayout::*)(QDomElement, Model::ScopeWrapperItem*, std::set<std::string>&)> sElements;

			InspectorLayoutData *mData;
		};
		
		class InspectorLayoutData : public QObject
		{
			Q_OBJECT
		public:
			InspectorLayoutData();

			InspectorLayout * getLayout(const QString&name);

			void setModel(Model::Inspector *inspector);

			void drawScope(Model::ScopeWrapperItem *scope);

			void drawRemainingMembers(Model::ScopeWrapperItem *scope, std::set<std::string> &drawn);
			void drawValue(QDomElement element, Model::ValueItem *item);

			Model::ScopeWrapperItem *registerIndex(Engine::InvScopePtr ptr);
			Model::ScopeWrapperItem *updateIndex(Engine::InvScopePtr from, Engine::InvScopePtr to);
			void unregisterIndex(Engine::InvScopePtr ptr);

		private:
			std::map<QString, InspectorLayout> mLayouts;
			std::map<QString, InspectorLayout*> mAssociations;

			std::map<Engine::InvScopePtr, size_t> mScopes;

			QFileSystemWatcher mFileWatcher;

			Model::Inspector *mInspector;
		};

	}
}