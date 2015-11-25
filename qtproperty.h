#ifndef QTPROPERTY_H
#define QTPROPERTY_H

#include <QObject>
#include <QVector>
#include <QVariant>
#include <QMap>

class QtProperty;
typedef QVector<QtProperty*>    QtPropertyList;
typedef QMap<QString, QVariant> QtPropertyAttributes;

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

    QtProperty(int type, QObject *parent);
    virtual ~QtProperty();

    int getType() const { return type_; }
    QtProperty* getParent() { return parent_; }

    void setName(const QString &name);
    const QString& getName() const { return name_; }

    void setTitle(const QString &title);
    const QString& getTitle() const;

    virtual void setValue(const QVariant &value);
    const QVariant& getValue() const { return value_; }

    virtual void setAttribute(const QString &name, const QVariant &value);
    QVariant getAttribute(const QString &name) const;
    QtPropertyAttributes& getAttributes(){ return attributes_; }

    void addChild(QtProperty *child);
    void removeChild(QtProperty *child);
    void removeAllChildren();
    void removeFromParent();

    int indexChild(const QtProperty *child) const;
    QtProperty* findChild(const QString &name);
    QtPropertyList& getChildren(){ return children_; }

    virtual bool hasValue() const { return true; }
    virtual bool isModified() const { return false; }

signals:
    void signalValueChange(QtProperty *property);
    void signalAddChild(QtProperty *property, QtProperty *child);
    void signalRemoveChild(QtProperty *property, QtProperty *child);

protected:
    virtual void onChildAdd(QtProperty *child);
    virtual void onChildRemove(QtProperty *child);

    int                 type_;
    QString             name_;
    QString             title_;
    QString             tips_;
    QVariant            value_;

    QtProperty*         parent_;
    QtPropertyList      children_;

    QMap<QString, QVariant> attributes_;

    bool                visible_;
};


/********************************************************************/
class QtGroupProperty : public QtProperty
{
    Q_OBJECT
public:
    QtGroupProperty(int type, QObject *parent);

protected slots:
    virtual void slotChildValueChange(QtProperty *property) = 0;

protected:
    virtual void onChildAdd(QtProperty *child);
    virtual void onChildRemove(QtProperty *child);
};


/********************************************************************/
class QtListProperty : public QtGroupProperty
{
    Q_OBJECT
public:
    QtListProperty(int type, QObject *parent);

    virtual void setValue(const QVariant &value);

protected slots:
    virtual void slotChildValueChange(QtProperty *property);
};


/********************************************************************/
class QtDictProperty : public QtGroupProperty
{
    Q_OBJECT
public:
    QtDictProperty(int type, QObject *parent);

    virtual void setValue(const QVariant &value);

protected slots:
    virtual void slotChildValueChange(QtProperty *property);
};

#endif // QTPROPERTY_H
