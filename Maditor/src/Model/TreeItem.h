
#pragma once

#include <QList>
#include <QVariant>
#include <QMenu>
#include <qstandarditem>
#include <functional>

namespace Maditor {
	namespace Model {

class TreeItem
{
public:
	explicit TreeItem(TreeItem *parent = 0);

	int childCount();
	int index();
	TreeItem *child(int i) const;

	TreeItem *parentItem();

	void extendContextMenu(QMenu &menu);

protected:
	void setContextMenuItems(std::list<std::pair<QString, std::function<void()>>> &&contextMenuItems);


private:
	std::list<TreeItem*> mChildren;
	QList<QVariant> mItemData;
	TreeItem *mParentItem;

	std::list<std::pair<QString, std::function<void()>>> mContextMenuItems;

	QIcon mIcon;
};

	}
}

