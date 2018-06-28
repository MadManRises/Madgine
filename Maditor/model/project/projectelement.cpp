#include "maditormodellib.h"

#include "projectelement.h"

#include "project.h"
#include "xmlexception.h"

namespace Maditor {
	namespace Model {



		ProjectElement::ProjectElement(const QString & name, const QString &type, ProjectElement *parent) :
			mRootElement(parent->mRootElement.ownerDocument().createElement(type)),
			mName(name),
			mParent(parent)
		{
			mRootElement.setAttribute("name", mName);
			parent->mRootElement.appendChild(mRootElement);
		}
		ProjectElement::ProjectElement(const QString & name, const QString & type, QDomDocument &doc) :			
			mRootElement(doc.createElement(type)),
			mName(name),
			mParent(nullptr)
		{
			mRootElement.setAttribute("name", mName);
			doc.appendChild(mRootElement);
		}
		ProjectElement::ProjectElement(QDomElement data, ProjectElement * parent) :
			mRootElement(data),
			mName(data.attribute("name")),
			mParent(parent)
		{
		}

		ProjectElement::ProjectElement(const QString & name, ProjectElement * parent) :
			mName(name),
			mParent(parent)
		{
		}

		ProjectElement::~ProjectElement()
		{
			if (mParent)
				mParent->mRootElement.removeChild(mRootElement);
		}

		const QString &ProjectElement::name() const
		{
			return mName;
		}

		QDir ProjectElement::path() const
		{
			return parentItem()->path();
		}

		Project * ProjectElement::project()
		{
			return parentItem()->project();
		}


		QDomElement & ProjectElement::element()
		{
			return mRootElement;
		}

		QDomElement ProjectElement::createElement(const QString & name)
		{
			QDomElement el = mRootElement.ownerDocument().createElement(name);
			mRootElement.appendChild(el);
			return el;
		}

		QDomElement ProjectElement::uniqueChildElement(const QString& name)
		{
			QDomElement e = mRootElement.firstChildElement(name);
			if (e.isNull())
				throw XmlException(Engine::Database::message("Missing Child \"", "\" in Node \"", "\"!")(name.toStdString(), mName.toStdString()));
			if (!e.nextSiblingElement(name).isNull())
				throw XmlException(Engine::Database::message("Multiple Children with tag \"", "\" in Node \"", "\"!")(name.toStdString(), mName.toStdString()));
			return e;
		}

		ProjectElement * ProjectElement::child(int i)
		{
			throw 0;
		}

		bool ProjectElement::save()
		{
			if (!storeData())
				return false;
			for (int i = 0; i < childCount(); ++i) {
				if (!child(i)->save())
					return false;
			}
			return true;
		}

		void ProjectElement::discardChanges()
		{
			restoreData();
		}

		void ProjectElement::writeImpl()
		{
			for (int i = 0; i < childCount(); ++i) {
				child(i)->writeImpl();
			}
			writeData();
		}

		bool ProjectElement::storeData()
		{
			return true;
		}

		void ProjectElement::restoreData()
		{
		}

		void ProjectElement::writeData()
		{
		}

		int ProjectElement::intAttribute(const QString & name)
		{
			return std::stoi(mRootElement.attribute(name).toStdString());
		}

		int ProjectElement::intAttribute(const QString & name, int defaultValue)
		{
			return mRootElement.hasAttribute(name) ? intAttribute(name) : defaultValue;
		}

		bool ProjectElement::boolAttribute(const QString & name)
		{
			return mRootElement.attribute(name) != "0";
		}

		QString ProjectElement::stringAttribute(const QString & name)
		{
			return mRootElement.attribute(name);
		}

		void ProjectElement::dirty()
		{
			project()->setDirtyFlag(true);
		}

		ProjectElement * ProjectElement::parentItem() const
		{
			return mParent;
		}

		QVariant ProjectElement::cellData(int col) const
		{
			return mName;
		}

		std::optional<QModelIndex> ProjectElement::modelIndex()
		{
			if (parentItem())
			{
				if (parentIndex() == -1)
				{
					return {};
				}
				else {
					std::optional<QModelIndex> parentI = parentItem()->modelIndex();
					if (parentI) {
						return project()->model()->index(parentIndex(), 0, parentI.value());
					}
					else
					{
						return {};
					}
				}
			}
			else
			{
				return QModelIndex();
			}

		}

		bool ProjectElement::beginInsertRows(size_t first, size_t last)
		{
			std::optional<QModelIndex> parentI = modelIndex();
			if (parentI) {
				project()->model()->beginInsertRows(parentI.value(), first, last);
				return true;
			}
			else
			{
				return false;
			}
		}

		void ProjectElement::endInsertRows(bool begun)
		{
			if (begun)
			{
				project()->model()->endInsertRows();
			}
		}

		bool ProjectElement::beginRemoveRows(size_t first, size_t last)
		{
			std::optional<QModelIndex> parentI = modelIndex();
			if (parentI) {
				project()->model()->beginRemoveRows(parentI.value(), first, last);
				return true;
			}
			else
			{
				return false;
			}
		}

		void ProjectElement::endRemoveRows(bool begun)
		{
			if (begun)
			{
				project()->model()->endRemoveRows();
			}
		}



	}
}
