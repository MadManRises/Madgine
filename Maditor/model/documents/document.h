#pragma once


namespace Maditor {
	namespace Model {

class MADITOR_MODEL_EXPORT Document : public QObject
{
	Q_OBJECT
public:
	Document(const QString &name);
	virtual ~Document() = default;	

	virtual void destroy();

	virtual void save();
	virtual void discardChanges();

	bool getDirtyFlag();

	QString getQualifiedName();
	const QString &getName();

	void setDirtyFlag(bool flag);

signals:
	void destroySignal(Document *);

	void dirtyFlagChanged(bool);


private:
	bool mDirtyFlag;
	QString mName;

};

	}
}

