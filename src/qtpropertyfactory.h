#ifndef QTPROPERTYMANAGER_H
#define QTPROPERTYMANAGER_H

#include <QObject>
#include <QMap>
#include "qtpropertytype.h"

class QtProperty;
class QtPropertyFactory;

typedef QtProperty*(*QtPropertyCreator)(QtPropertyType::Type type, QtPropertyFactory *factory);

class QtPropertyFactory : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyFactory(QObject *parent = 0);

    QtProperty* createProperty(QtPropertyType::Type type);

    void registerCreator(QtPropertyType::Type type, QtPropertyCreator method);

    template<typename T>
    void registerCreator(QtPropertyType::Type type);

private:
    template<typename T>
    static QtProperty* internalCreator(QtPropertyType::Type type, QtPropertyFactory *factory);

    typedef QMap<QtPropertyType::Type, QtPropertyCreator> CreatorMap;
    CreatorMap      propertyCreator_;
};

template<typename T>
void QtPropertyFactory::registerCreator(QtPropertyType::Type type)
{
    return this->registerCreator(type, this->internalCreator<T>);
}

template<typename T>
/*static*/ QtProperty* QtPropertyFactory::internalCreator(QtPropertyType::Type type, QtPropertyFactory *factory)
{
    return new T(type, factory);
}

#endif // QTPROPERTYMANAGER_H
