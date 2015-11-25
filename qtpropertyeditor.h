#ifndef QTPROPERTYEDITOR_H
#define QTPROPERTYEDITOR_H

#include <QObject>
class QtProperty;
class QWidget;
class QSpinBox;

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

protected:

    QtProperty*         property_;
};

class QtSpinBoxEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtSpinBoxEditor(QtProperty *property);
    ~QtSpinBoxEditor();

    virtual QWidget* createEditor(QWidget *parent);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void slotEditorValueChange(int value);
    void slotEditorDestory(QObject *object);

private:
    int                 value_;
    QSpinBox*           editor_;

};

#endif // QTPROPERTYEDITOR_H
