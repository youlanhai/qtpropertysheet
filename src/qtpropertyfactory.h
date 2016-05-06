#ifndef QTPROPERTYMANAGER_H
#define QTPROPERTYMANAGER_H

#include <QObject>
#include <QMap>

class QtProperty;
class QtPropertyFactory;

typedef QtProperty*(*QtPropertyCreator)(int type, QtPropertyFactory *factory);

class QtPropertyFactory : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyFactory(QObject *parent = 0);

    QtProperty* createProperty(int type);

    void registerCreator(int type, QtPropertyCreator method);

    template<typename T>
    void registerCreator(int type);

private:
    template<typename T>
    static QtProperty* internalCreator(int type, QtPropertyFactory *factory);

    typedef QMap<int, QtPropertyCreator> CreatorMap;
    CreatorMap      propertyCreator_;
};

template<typename T>
void QtPropertyFactory::registerCreator(int type)
{
    return this->registerCreator(type, this->internalCreator<T>);
}

template<typename T>
/*static*/ QtProperty* QtPropertyFactory::internalCreator(int type, QtPropertyFactory *factory)
{
    return new T(type, factory);
}

#endif // QTPROPERTYMANAGER_H
