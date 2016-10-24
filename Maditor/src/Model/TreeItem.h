
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
	virtual ~TreeItem() = default;

	virtual int childCount() = 0;
	virtual int parentIndex();
	virtual TreeItem *child(int i) = 0;

	virtual TreeItem *parentItem() = 0;

	virtual QVariant data(int col) const = 0;

	virtual QVariant icon() const;

	void extendContextMenu(QMenu &menu);

protected:
	void setContextMenuItems(std::list<std::pair<QString, std::function<void()>>> &&contextMenuItems);


private:
	std::list<std::pair<QString, std::function<void()>>> mContextMenuItems;

	
};

	}
}

