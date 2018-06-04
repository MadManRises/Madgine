#ifndef VALUETYPEEDIT_H
#define VALUETYPEEDIT_H

class VectorWidget : public QWidget {
public:
	VectorWidget(int size);

	void setValue(const std::array<float, 4> &v);
	void setValue(const Engine::Vector3 &v);
	void setValue(const Engine::Vector2 &v);

	void setReadOnly(bool b);

private:
	std::vector<QDoubleSpinBox*> mBoxes;
};

class MADITOR_VIEW_EXPORT ValueTypeEdit : public QWidget
{
    Q_OBJECT

public:
    explicit ValueTypeEdit(QWidget *parent = 0);
    ~ValueTypeEdit();

	void setType(Engine::ValueType::Type type);
	void setValue(const Engine::ValueType &value);

	const Engine::ValueType &getValue();

	void setReadOnly(bool b);

signals:
	void scopeLinkClicked(Engine::InvScopePtr ptr);

private:
	void onTypeChanged();
	void onValueChanged();

	void followScopeLink();

private:
	union {
		QLabel *mNullLabel;
		QSpinBox *mIntBox;
		QSpinBox *mUIntBox;
		VectorWidget *mVectorBox;
		QPushButton *mScopeField;
		QCheckBox *mBoolBox;
		QLineEdit *mStringBox;
	};
	QWidget *mInputWidget;
	QMenu *mTypeMenu;
	QToolButton *mOuterMenu;
	QGridLayout *mLayout;

	Engine::ValueType mValue;

	bool mReadOnly;

	Q_PROPERTY(Engine::ValueType value READ getValue WRITE setValue USER true);

};

#endif // VALUETYPEEDIT_H
