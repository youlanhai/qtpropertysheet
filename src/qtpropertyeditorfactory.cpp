#include "qtpropertyeditorfactory.h"
#include "qtproperty.h"
#include "qtpropertyeditor.h"

template <typename T>
static QtPropertyEditor* internalCreator(QtProperty *property)
{
    return new T(property);
}

QtPropertyEditorFactory::QtPropertyEditorFactory(QObject *parent)
    : QObject(parent)
{
#define REGISTER_CREATOR(TYPE, CLASS) \
    registerCreator(TYPE, internalCreator<CLASS>)

    REGISTER_CREATOR(QtProperty::TYPE_INT, QtIntSpinBoxEditor);
    REGISTER_CREATOR(QtProperty::TYPE_DOUBLE, QtDoubleSpinBoxEditor);
    REGISTER_CREATOR(QtProperty::TYPE_STRING, QtStringEditor);

#undef QtSpinBoxEditor
}

QWidget* QtPropertyEditorFactory::createEditor(QtProperty *property, QWidget *parent)
{
    QtPropertyEditor *propertyEditor = createPropertyEditor(property);
    if(propertyEditor != NULL)
    {
        return propertyEditor->createEditor(parent);
    }
    return NULL;
}

QtPropertyEditor* QtPropertyEditorFactory::createPropertyEditor(QtProperty *property)
{
    QtPropertyEditorCreator method = creators_.value(property->getType());
    if(method != NULL)
    {
        return method(property);
    }
    return NULL;
}

void QtPropertyEditorFactory::registerCreator(int type, QtPropertyEditorCreator method)
{
    creators_[type] = method;
}
