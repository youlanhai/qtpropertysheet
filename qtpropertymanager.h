#ifndef QTPROPERTYMANAGER_H
#define QTPROPERTYMANAGER_H

#include <QObject>

class QtProperty;

class QtPropertyManager : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyManager(QObject *parent = 0);

    virtual QtProperty* addProperty(int type);
    virtual QtProperty* createProperty(int type);

    void setValue(const QString &name, const QVariant &value);

signals:
    void propertyInserted(QtProperty *property, QtProperty *parent, QtProperty *after);
    void propertyChanged(QtProperty *property);
    void propertyRemoved(QtProperty *property, QtProperty *parent);
    void propertyDestroyed(QtProperty *property);

public slots:
};

#endif // QTPROPERTYMANAGER_H
