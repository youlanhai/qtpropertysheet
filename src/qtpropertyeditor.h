#ifndef QTPROPERTYEDITOR_H
#define QTPROPERTYEDITOR_H

#include <QObject>
#include <QColor>

class QtProperty;
class QWidget;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QToolButton;

class QtColorEditWidget;
class QtBoolEdit;
class QxtCheckComboBox;

// QtPropertyEditor will be destroied when QtEditor destroied.
class QtPropertyEditor : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyEditor(QtProperty *property);
    virtual ~QtPropertyEditor();

    virtual QWidget* createEditor(QWidget *parent) = 0;

public slots:
    virtual void onPropertyValueChange(QtProperty *property) = 0;
    virtual void onPropertyDestory(QObject *object);
    virtual void slotEditorDestory(QObject *object);

protected:

    QtProperty*         property_;
};


class QtIntSpinBoxEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtIntSpinBoxEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void slotEditorValueChange(int value);
    void slotSetAttribute(QtProperty *property, const QString &name);

private:
    int                 value_;
    QSpinBox*           editor_;

};

class QtDoubleSpinBoxEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtDoubleSpinBoxEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void slotEditorValueChange(double value);
    void slotSetAttribute(QtProperty *property, const QString &name);

private:
    double              value_;
    QDoubleSpinBox*     editor_;

};

class QtStringEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtStringEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void slotEditFinished();

private:
    QString             value_;
    QLineEdit*          editor_;

};

class QtEnumEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtEnumEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    virtual void slotEditorValueChange(int index);
    void slotSetAttribute(QtProperty *property, const QString &name);

private:
    int                 value_;
    QComboBox*          editor_;
};

class QStandardItem;
class QtFlagEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtFlagEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent);

    void setValueToEditor(int value);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void checkedItemsChanged(const QStringList& items);
    void slotSetAttribute(QtProperty *property, const QString &name);

private:
    int                 value_;
    QxtCheckComboBox*   editor_;
    QStringList         flagNames_;
};

class QtBoolEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtBoolEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    virtual void slotEditorValueChange(bool value);

protected:
    bool                value_;
    QtBoolEdit*         editor_;
};

class QtColorEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtColorEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    virtual void slotEditorValueChange(const QColor &color);

protected:
    QColor              value_;
    QtColorEditWidget*  editor_;
};

class QtFileEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtFileEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent);

public slots:
    void slotButtonClicked();
    void slotEditingFinished();

    virtual void onPropertyValueChange(QtProperty *property);

private:
    void setValue(const QString &value);

    QString         value_;
    QWidget*        editor_;
    QLineEdit*      input_;
    QToolButton*    button_;
};

#endif // QTPROPERTYEDITOR_H
