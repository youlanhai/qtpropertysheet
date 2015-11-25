#ifndef QTPROPERTYMANAGER_H
#define QTPROPERTYMANAGER_H

#include <QObject>
#include <QMap>

class QtProperty;

typedef QtProperty*(*QtPropertyCreator)(int type, QObject *parent);

class QtPropertyManager : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyManager(QObject *parent = 0);

    virtual QtProperty* createProperty(int type, QObject *parent);

    void registerCreator(int type, QtPropertyCreator method);

private:
    typedef QMap<int, QtPropertyCreator> CreatorMap;
    CreatorMap      propertyCreator_;
};

#endif // QTPROPERTYMANAGER_H
