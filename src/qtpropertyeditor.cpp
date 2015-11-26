#include "qtpropertyeditor.h"
#include "qtproperty.h"
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
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
    property_ = NULL;
    delete this;
}

void QtPropertyEditor::slotEditorDestory(QObject * /*object*/)
{
    delete this;
}

/********************************************************************/
QtIntSpinBoxEditor::QtIntSpinBoxEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(0)
{
    value_ = property_->getValue().toInt();
}

QWidget* QtIntSpinBoxEditor::createEditor(QWidget *parent)
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

void QtIntSpinBoxEditor::slotEditorValueChange(int value)
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

void QtIntSpinBoxEditor::onPropertyValueChange(QtProperty* /*property*/)
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
/********************************************************************/
QtDoubleSpinBoxEditor::QtDoubleSpinBoxEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(0)
{
    value_ = property_->getValue().toDouble();
}

QWidget* QtDoubleSpinBoxEditor::createEditor(QWidget *parent)
{
    if(editor_ == 0)
    {
        editor_ = new QDoubleSpinBox(parent);
        editor_->setKeyboardTracking(false);
        editor_->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
        editor_->setValue(value_);

        connect(editor_, SIGNAL(valueChanged(double)), this, SLOT(slotEditorValueChange(double)));
        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    }
    return editor_;
}

void QtDoubleSpinBoxEditor::slotEditorValueChange(double value)
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

void QtDoubleSpinBoxEditor::onPropertyValueChange(QtProperty* /*property*/)
{
    int value = property_->getValue().toDouble();
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
/********************************************************************/
QtStringEditor::QtStringEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = property->getValue().toString();
}

QWidget* QtStringEditor::createEditor(QWidget *parent)
{
    if(editor_ == NULL)
    {
        editor_ = new QLineEdit(parent);
        editor_->setText(value_);
        connect(editor_, SIGNAL(editingFinished()), this, SLOT(slotEditFinished()));
        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    }
    return editor_;
}

void QtStringEditor::onPropertyValueChange(QtProperty *property)
{
    QString value = property->getValue().toString();
    if(value == value_)
    {
        return;
    }

    value_ = value;
    if(editor_ != NULL)
    {
        editor_->setText(value_);
    }
}

void QtStringEditor::slotEditFinished()
{
    QString text = editor_->text();
    if(value_ != text)
    {
        value_ = text;
        if(property_ != 0)
        {
            property_->setValue(value_);
        }
    }
}
