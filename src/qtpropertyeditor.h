#ifndef QTPROPERTYEDITOR_H
#define QTPROPERTYEDITOR_H

#include <QObject>
class QtProperty;
class QWidget;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;

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
    QLineEdit*        editor_;

};

#endif // QTPROPERTYEDITOR_H
