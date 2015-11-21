#ifndef QTPROPERTYMANAGER_H
#define QTPROPERTYMANAGER_H

#include <QObject>

class QtProperty;

class QtPropertyManager : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyManager(QObject *parent = 0);

    void setPropertyValue(const QString &name, const QVariant &value);

signals:
    void signalPropertyChange(QtProperty *property);

public slots:
};

#endif // QTPROPERTYMANAGER_H
