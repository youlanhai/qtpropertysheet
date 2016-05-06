#ifndef QTPROPERTYMANAGER_H
#define QTPROPERTYMANAGER_H

#include <QObject>
#include <QMap>

class QtProperty;

typedef QtProperty*(*QtPropertyCreator)(int type, QObject *parent);

class QtPropertyFactory : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyFactory(QObject *parent = 0);

    QtProperty* createProperty(int type, QObject *parent);

    void registerCreator(int type, QtPropertyCreator method);

    template<typename T>
    void registerCreator(int type);

private:
    template<typename T>
    static QtProperty* internalCreator(int type, QObject *parent);

    typedef QMap<int, QtPropertyCreator> CreatorMap;
    CreatorMap      propertyCreator_;
};

template<typename T>
void QtPropertyFactory::registerCreator(int type)
{
    return this->registerCreator(type, this->internalCreator<T>);
}

template<typename T>
/*static*/ QtProperty* QtPropertyFactory::internalCreator(int type, QObject *parent)
{
    return new T(type, parent);
}

#endif // QTPROPERTYMANAGER_H
