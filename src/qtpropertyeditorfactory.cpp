#include "qtpropertyeditorfactory.h"
#include "qtproperty.h"
#include "qtpropertyeditor.h"


QtPropertyEditorFactory::QtPropertyEditorFactory(QObject *parent)
    : QObject(parent)
{
#define REGISTER_CREATOR(TYPE, CLASS) \
    registerCreator<CLASS>(TYPE)

    REGISTER_CREATOR(QtProperty::TYPE_INT, QtIntSpinBoxEditor);
    REGISTER_CREATOR(QtProperty::TYPE_DOUBLE, QtDoubleSpinBoxEditor);
    REGISTER_CREATOR(QtProperty::TYPE_STRING, QtStringEditor);
    REGISTER_CREATOR(QtProperty::TYPE_ENUM, QtEnumEditor);
    REGISTER_CREATOR(QtProperty::TYPE_BOOL, QtBoolEditor);
    REGISTER_CREATOR(QtProperty::TYPE_COLOR, QtColorEditor);
    REGISTER_CREATOR(QtProperty::TYPE_FLAG, QtFlagEditor);
    REGISTER_CREATOR(QtProperty::TYPE_FILE, QtFileEditor);
    REGISTER_CREATOR(QtProperty::TYPE_DYNAMIC_ITEM, QtDynamicItemEditor);
    REGISTER_CREATOR(QtProperty::TYPE_ENUM_PAIR, QtEnumPairEditor);

#undef QtSpinBoxEditor
}

QWidget* QtPropertyEditorFactory::createEditor(QtProperty *property, QWidget *parent)
{
    QtPropertyEditor *propertyEditor = createPropertyEditor(property);
    if(propertyEditor != NULL)
    {
        return propertyEditor->createEditor(parent, this);
    }
    return NULL;
}

QtPropertyEditor* QtPropertyEditorFactory::createPropertyEditor(QtProperty *property, int type)
{
    if(type == QtProperty::TYPE_NONE)
    {
        type = property->getType();
    }

    QtPropertyEditorCreator method = creators_.value(type);
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
