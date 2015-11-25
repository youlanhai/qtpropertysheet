#include "qtpropertymanager.h"
#include "qtproperty.h"

QtPropertyManager::QtPropertyManager(QObject *parent)
    : QObject(parent)
{
}

QtProperty* QtPropertyManager::createProperty(int type, QObject *parent)
{
    QtPropertyCreator method = propertyCreator_.value(type);
    if(method != NULL)
    {
        return method(type, parent);
    }
    return new QtProperty(type, parent);
}

void QtPropertyManager::registerCreator(int type, QtPropertyCreator method)
{
    propertyCreator_[type] = method;
}
