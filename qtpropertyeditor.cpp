#include "qtpropertyeditor.h"
#include "qtproperty.h"
#include <QSpinBox>

QtPropertyEditor::QtPropertyEditor(QtProperty *property, QObject *parent)
    : QObject(parent)
    , property_(property)
{

}

void QtPropertyEditor::onPropertyValueChange()
{

}

void QtPropertyEditor::onPropertyDestory(QObject * /*object*/)
{
    property_ = 0;
}


QtSpinBoxEditor::QtSpinBoxEditor(QtProperty *property, QObject *parent)
    : QtPropertyEditor(property, parent)
    , editor_(0)
{
    value_ = property_->getValue().toInt();
}

QtSpinBoxEditor::~QtSpinBoxEditor()
{
}

QWidget* QtSpinBoxEditor::createEditor(QWidget *parent)
{
    if(editor_ == 0)
    {
        editor_ = new QSpinBox(parent);
        editor_->setValue(value_);
        connect(editor_, SIGNAL(valueChanged(int)), this, SLOT(slotEditorValueChange(int)));
        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    }
    return editor_;
}

void QtSpinBoxEditor::slotEditorValueChange(int value)
{
    if(value == value_)
    {
        return;
    }
    value_ = value;

    if(property_ != 0)
    {
        property_->setValue(value);
    }
}

void QtSpinBoxEditor::slotEditorDestory(QObject * /*object*/)
{
    editor_ = 0;
}

void QtSpinBoxEditor::onPropertyValueChange()
{
    int value = property_->getValue().toInt();
    if(value == value_)
    {
        return;
    }

    value_ = value;
    if(editor_ != 0)
    {
        editor_->setValue(value_);
    }
}
