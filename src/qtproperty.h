#ifndef QTPROPERTY_H
#define QTPROPERTY_H

#include <QObject>
#include <QVector>
#include <QVariant>
#include <QMap>
#include <QIcon>

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
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_GROUP,
        TYPE_LIST,
        TYPE_DICT,
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
    virtual const QVariant& getValue() const { return value_; }

    virtual QString getValueString() const;
    virtual QIcon getValueIcon() const;

    virtual void setAttribute(const QString &name, const QVariant &value);
    QVariant getAttribute(const QString &name) const;
    QtPropertyAttributes& getAttributes(){ return attributes_; }

    void addChild(QtProperty *child);
    void removeChild(QtProperty *child);
    void removeAllChildren();
    void removeFromParent();

    QtPropertyList& getChildren(){ return children_; }
    int indexChild(const QtProperty *child) const;
    virtual QtProperty* findChild(const QString &name);

    virtual void setChildValue(const QString &name, const QVariant &value);

    virtual bool hasValue() const { return true; }
    virtual bool isModified() const { return false; }

    void setVisible(bool visible){ visible_ = visible; }
    bool isVisible() const { return visible_; }

    void setSelfVisible(bool visible){ selfVisible_ = visible; }
    bool isSelfVisible() const { return selfVisible_; }

signals:
    void signalValueChange(QtProperty *property);
    void signalPropertyInserted(QtProperty *property, QtProperty *parent);
    void signalPropertyRemoved(QtProperty *property, QtProperty *parent);
    void signalAttributeChange(QtProperty *property, const QString &name);

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
    bool                selfVisible_;
};

/********************************************************************/
class QtContainerProperty : public QtProperty
{
    Q_OBJECT
public:
    QtContainerProperty(int type, QObject *parent);
    virtual bool hasValue() const { return false; }

protected slots:
    virtual void slotChildValueChange(QtProperty *property) = 0;

protected:
    virtual void onChildAdd(QtProperty *child);
    virtual void onChildRemove(QtProperty *child);
};


/********************************************************************/
class QtListProperty : public QtContainerProperty
{
    Q_OBJECT
public:
    QtListProperty(int type, QObject *parent);

    virtual void setValue(const QVariant &value);
    virtual QString getValueString() const;

protected slots:
    virtual void slotChildValueChange(QtProperty *property);
};


/********************************************************************/
class QtDictProperty : public QtContainerProperty
{
    Q_OBJECT
public:
    QtDictProperty(int type, QObject *parent);

    virtual void setValue(const QVariant &value);

protected slots:
    virtual void slotChildValueChange(QtProperty *property);
};


/**
 * @brief The QtGroupProperty class
 *
 * QtGroupProperty doesn't have value, it emits the children's value change event
 * to it's listener directly.
 *
 * NOTICE: QtGroupProperty can only be root or child of another QtGroupProperty,
 * Adding QtGroupProperty as child of any other property, will cause unexpected problem.
 */
class QtGroupProperty : public QtContainerProperty
{
    Q_OBJECT
public:
    QtGroupProperty(int type, QObject *parent);

    virtual void setValue(const QVariant &value);

    virtual QtProperty* findChild(const QString &name);
    virtual void setChildValue(const QString &name, const QVariant &value);

protected slots:
    virtual void slotChildValueChange(QtProperty *property);
};



/********************************************************************/
class QtEnumProperty : public QtProperty
{
    Q_OBJECT
public:
    QtEnumProperty(int type, QObject *parent);
    virtual QString getValueString() const;
};

/********************************************************************/
class QtBoolProperty : public QtProperty
{
    Q_OBJECT
public:
    QtBoolProperty(int type, QObject *parent);
    virtual QString getValueString() const;
    virtual QIcon getValueIcon() const;
};

/********************************************************************/
class QtColorProperty : public QtProperty
{
    Q_OBJECT
public:
    QtColorProperty(int type, QObject *parent);

    virtual QString getValueString() const;
    virtual QIcon getValueIcon() const;
};

#endif // QTPROPERTY_H
