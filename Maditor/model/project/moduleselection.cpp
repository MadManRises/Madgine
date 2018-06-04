#include "maditormodellib.h"

#include "moduleselection.h"

#include "modulelist.h"

#include "applicationconfig.h"

#include "project.h"

namespace Maditor {
	namespace Model {

		ModuleSelection::ModuleSelection(ApplicationConfig *parent) :
			ProjectElement("ModuleSelection", parent),
			mModules(parent->project()->moduleList()),
			mParent(parent)
		{

		}

		ModuleSelection::ModuleSelection(QDomElement element, ApplicationConfig * parent) :
			ProjectElement(element, parent),
			mModules(parent->project()->moduleList()),
			mParent(parent)
		{
		}

		Q_INVOKABLE int ModuleSelection::rowCount(const QModelIndex & parent) const
		{
			return mModules->childCount();
		}

		Q_INVOKABLE QVariant ModuleSelection::data(const QModelIndex & index, int role) const
		{
			if (!index.isValid())
				return QVariant();

			QString name = mModules->child(index.row())->name();
			switch (role) {
			case Qt::CheckStateRole:
				return std::find(mExcludedModules.begin(), mExcludedModules.end(), name) == mExcludedModules.end() ? Qt::Checked : Qt::Unchecked;
			case Qt::DisplayRole:
				return name;
			default:
				return QVariant();
			}
			
		}

		void ModuleSelection::itemClicked(const QModelIndex & index)
		{
			QString name = mModules->child(index.row())->name();
			auto it = std::find(mExcludedModules.begin(), mExcludedModules.end(), name);
			if (it == mExcludedModules.end()) {
				mExcludedModules.push_back(name);
			}
			else {
				mExcludedModules.erase(it);
			}
			emit dataChanged(index, index, { Qt::CheckStateRole });
			mParent->setDirtyFlag(true);
		}

		bool ModuleSelection::storeData()
		{
			for (QDomElement el = element().firstChildElement("exclude"); !el.isNull(); el = element().firstChildElement("exclude")) {
				element().removeChild(el);
			}
			for (const QString &name : mExcludedModules) {
				QDomElement el = element().ownerDocument().createElement("exclude");
				el.appendChild(element().ownerDocument().createTextNode(name));
				element().appendChild(el);
			}
			return true;
		}

		void ModuleSelection::restoreData()
		{
			mExcludedModules.clear();
			for (QDomElement el = element().firstChildElement("exclude"); !el.isNull(); el = el.nextSiblingElement("exclude")) {
				mExcludedModules << el.text();
			}
		}

		QStringList ModuleSelection::libraries()
		{
			QSet<QString> libraries;
			for (const std::unique_ptr<Module> &mod : *mModules) {
				if (!mExcludedModules.contains(mod->name())) {
					libraries.unite(QSet<QString>::fromList(mod->libraryDependencies()));
				}
			}
			return libraries.toList();
		}

		QStringList ModuleSelection::files()
		{
			QStringList files;
			for (const std::unique_ptr<Module> &mod : *mModules) {
				if (!mExcludedModules.contains(mod->name())) {
					for (const QString &f : mod->files()) {
						files << (mModules->path() + mod->name() + "/" + f);
					}
				}
			}
			return files;
		}

		bool ModuleSelection::isEnabled(const QString & name)
		{
			return !mExcludedModules.contains(name);
		}

	}
}
