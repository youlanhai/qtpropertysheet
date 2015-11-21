#ifndef QTPROPERTY_H
#define QTPROPERTY_H

#include <QObject>
#include <QVector>
#include <QVariant>
#include <QMap>

class QtProperty;
typedef QVector<QtProperty*>    QtPropertyList;
typedef QMap<QString, QVariant> QtPropertyAttributes;

class QtPropertyManager;


class QtProperty : public QObject
{
    Q_OBJECT
public:

    enum Type
    {
        TYPE_BOOL,
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_STRING,
        TYPE_LIST,
        TYPE_MAP,
        TYPE_GROUP,
        TYPE_ENUM,
        TYPE_FLAG,
        TYPE_COLOR,
        TYPE_USER = 256,
    };

    explicit QtProperty(int type, QObject *parent = 0);

    int getType() const { return type_; }
    QtProperty* getParent() { return parent_; }

    void setName(const QString &name);
    const QString& getName() const { return name_; }

    void setTitle(const QString &title);
    const QString& getTitle() const { return title_; }

    virtual void setValue(const QVariant &value);
    const QVariant& getValue() const { return value_; }

    void setAttribute(const QString &name, const QVariant &value);
    QVariant getAttribute(const QString &name) const;
    QtPropertyAttributes& getAttributes();

    void addChild(QtProperty *child);
    void removeChild(QtProperty *child);
    void removeFromParent();

    QtProperty* findChild(const QString &name);
    QtPropertyList& getChildren();

    void setManager(QtPropertyManager *manager);
    QtPropertyManager* getManager() { return manager_; }

    void notifyPropertyChange();
    virtual void onChildPopertyChange(QtProperty *property);
    virtual void setChildPropertyValue(const QString &name, const QVariant &value);

signals:
    void signalValueChange();

protected:
    int                 type_;
    QString             name_;
    QString             title_;
    QString             tips_;
    QVariant            value_;

    QtProperty*         parent_;
    QtPropertyList      children_;
    QtPropertyManager*  manager_;

    QMap<QString, QVariant> attributes_;
};

#endif // QTPROPERTY_H
