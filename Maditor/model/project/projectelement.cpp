#include "maditormodellib.h"

#include "projectelement.h"

#include "project.h"

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

		QModelIndex ProjectElement::ownIndex()
		{
			return parentItem() == nullptr ? QModelIndex() : project()->model()->index(parentIndex(), 0, parentItem()->ownIndex());
		}

		QString ProjectElement::path() const
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



	}
}
