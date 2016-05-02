#ifndef QTPROPERTYMANAGER_H
#define QTPROPERTYMANAGER_H

#include <QObject>
#include <QMap>

class QtProperty;
class QtPropertyFactory;

typedef QtProperty*(*QtPropertyCreator)(int type, QtPropertyFactory *parent);

class QtPropertyFactory : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyFactory(QObject *parent = 0);

    virtual QtProperty* createProperty(int type);

    void registerCreator(int type, QtPropertyCreator method);

private:
    typedef QMap<int, QtPropertyCreator> CreatorMap;
    CreatorMap      propertyCreator_;
};

#endif // QTPROPERTYMANAGER_H
