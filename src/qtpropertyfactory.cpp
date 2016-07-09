#include "qtpropertyfactory.h"
#include "qtproperty.h"


QtPropertyFactory::QtPropertyFactory(QObject *parent)
    : QObject(parent)
{
#define REGISTER_PROPERTY(TYPE, CLASS) registerCreator<CLASS>(TYPE)

    REGISTER_PROPERTY(QtPropertyType::LIST, QtListProperty);
    REGISTER_PROPERTY(QtPropertyType::DICT, QtDictProperty);
    REGISTER_PROPERTY(QtPropertyType::GROUP, QtGroupProperty);
    REGISTER_PROPERTY(QtPropertyType::ENUM, QtEnumProperty);
    REGISTER_PROPERTY(QtPropertyType::FLAG, QtFlagProperty);
    REGISTER_PROPERTY(QtPropertyType::BOOL, QtBoolProperty);
    REGISTER_PROPERTY(QtPropertyType::DOUBLE, QtDoubleProperty);
    REGISTER_PROPERTY(QtPropertyType::COLOR, QtColorProperty);
    REGISTER_PROPERTY(QtPropertyType::DYNAMIC_LIST, QtDynamicListProperty);
    REGISTER_PROPERTY(QtPropertyType::DYNAMIC_ITEM, QtDynamicItemProperty);

#undef REGISTER_PROPERTY
}

QtProperty* QtPropertyFactory::createProperty(QtPropertyType::Type type)
{
    QtPropertyCreator method = propertyCreator_.value(type);
    if(method != NULL)
    {
        return method(type, this);
    }
    
    // use default QtProperty
    return new QtProperty(type, this);
}

void QtPropertyFactory::registerCreator(QtPropertyType::Type type, QtPropertyCreator method)
{
    propertyCreator_[type] = method;
}
