#include "qtpropertymanager.h"
#include "qtproperty.h"

QtPropertyManager::QtPropertyManager(QObject *parent)
    : QObject(parent)
{

}

QtProperty* QtPropertyManager::addProperty(int type)
{
    QtProperty *property = createProperty(type);
    if(property != NULL)
    {

    }
    return property;
}

QtProperty* QtPropertyManager::createProperty(int type)
{
    return new QtProperty(type, this);
}
