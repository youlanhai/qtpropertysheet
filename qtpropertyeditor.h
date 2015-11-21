#ifndef QTPROPERTYEDITOR_H
#define QTPROPERTYEDITOR_H

#include <QObject>
class QtProperty;
class QWidget;
class QSpinBox;

class QtPropertyEditor : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyEditor(QtProperty *property, QObject *parent = 0);

    virtual QWidget* createEditor(QWidget *parent) = 0;

public slots:
    virtual void onPropertyValueChange();
    virtual void onPropertyDestory(QObject *object);

protected:

    QtProperty*         property_;
};

class QtSpinBoxEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtSpinBoxEditor(QtProperty *property, QObject *parent = 0);
    ~QtSpinBoxEditor();

    virtual QWidget* createEditor(QWidget *parent);

public slots:
    virtual void onPropertyValueChange();
    void slotEditorValueChange(int value);
    void slotEditorDestory(QObject *object);

private:
    int                 value_;
    QSpinBox*           editor_;

};

#endif // QTPROPERTYEDITOR_H
