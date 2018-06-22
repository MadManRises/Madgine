#pragma once


namespace Maditor {
	namespace Model {

class TreeItem
{
public:
	virtual ~TreeItem() = default;

	virtual int childCount() const;
	virtual int parentIndex();
	virtual TreeItem *child(int i) = 0;

	virtual TreeItem *parentItem() const = 0;

	virtual QVariant cellData(int col) const = 0;

	virtual QVariant icon() const;

	virtual void extendContextMenu(QMenu &menu);

	virtual void doubleClicked();

protected:
	void setContextMenuItems(std::map<QString, std::function<void()>> &&contextMenuItems);
	void addContextMenuItem(const QString &name, std::function<void()> f);
	void removeContextMenuItem(const QString &name);

	const std::map<QString, std::function<void()>> &getContextMenuItems();

private:
	std::map<QString, std::function<void()>> mContextMenuItems;

	
};

	}
}

