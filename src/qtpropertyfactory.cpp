#include "qtpropertyfactory.h"
#include "qtproperty.h"


QtPropertyFactory::QtPropertyFactory(QObject *parent)
    : QObject(parent)
{
#define REGISTER_PROPERTY(TYPE, CLASS) registerCreator<CLASS>(TYPE)

    REGISTER_PROPERTY(QtProperty::TYPE_LIST, QtListProperty);
    REGISTER_PROPERTY(QtProperty::TYPE_DICT, QtDictProperty);
    REGISTER_PROPERTY(QtProperty::TYPE_GROUP, QtGroupProperty);
    REGISTER_PROPERTY(QtProperty::TYPE_ENUM, QtEnumProperty);
    REGISTER_PROPERTY(QtProperty::TYPE_FLAG, QtFlagProperty);
    REGISTER_PROPERTY(QtProperty::TYPE_BOOL, QtBoolProperty);
    REGISTER_PROPERTY(QtProperty::TYPE_DOUBLE, QtDoubleProperty);
    REGISTER_PROPERTY(QtProperty::TYPE_COLOR, QtColorProperty);
    REGISTER_PROPERTY(QtProperty::TYPE_DYNAMIC_LIST, QtDynamicListProperty);
    REGISTER_PROPERTY(QtProperty::TYPE_DYNAMIC_ITEM, QtDynamicItemProperty);

#undef REGISTER_PROPERTY
}

QtProperty* QtPropertyFactory::createProperty(int type)
{
    QtPropertyCreator method = propertyCreator_.value(type);
    if(method != NULL)
    {
        return method(type, this);
    }
    return new QtProperty(type, this);
}

void QtPropertyFactory::registerCreator(int type, QtPropertyCreator method)
{
    propertyCreator_[type] = method;
}
