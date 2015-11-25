#include "qtpropertyeditor.h"
#include "qtproperty.h"
#include <QSpinBox>
#include <limits>

QtPropertyEditor::QtPropertyEditor(QtProperty *property)
    : property_(property)
{
    connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onPropertyValueChange(QtProperty*)));
    connect(property, SIGNAL(destroyed(QObject*)), this, SLOT(onPropertyDestory(QObject*)));

    printf("QtPropertyEditor construct: %ld\n", size_t(this));
    fflush(stdout);
}

QtPropertyEditor::~QtPropertyEditor()
{
    printf("QtPropertyEditor destruct: %ld\n", size_t(this));
    fflush(stdout);
}

void QtPropertyEditor::onPropertyDestory(QObject * /*object*/)
{
    delete this;
}


QtSpinBoxEditor::QtSpinBoxEditor(QtProperty *property)
    : QtPropertyEditor(property)
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
        editor_->setKeyboardTracking(false);
        editor_->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
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

void QtSpinBoxEditor::slotEditorDestory(QObject* /*object*/)
{
    editor_ = 0;
    delete this;
}

void QtSpinBoxEditor::onPropertyValueChange(QtProperty* /*property*/)
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
