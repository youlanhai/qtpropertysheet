#include "qtproperty.h"
#include "qtpropertymanager.h"
#include <cassert>
#include <algorithm>

QtProperty::QtProperty(int type, QObject *parent)
    : QObject(parent)
    , type_(type)
    , parent_(NULL)
{

}

QtProperty::~QtProperty()
{
    emit signalPropertyRemoved(this, parent_);

    removeAllChildren();
    removeFromParent();
}

void QtProperty::setName(const QString &name)
{
    name_ = name;
}

const QString& QtProperty::getTitle() const
{
    return title_.isEmpty() ? name_ : title_;
}

void QtProperty::setTitle(const QString &title)
{
    title_ = title;
}

void QtProperty::setValue(const QVariant &value)
{
    if(value_ != value)
    {
        value_ = value;
        emit signalValueChange(this);
    }
}

void QtProperty::setAttribute(const QString &name, const QVariant &value)
{
    attributes_[name] = value;
}

QVariant QtProperty::getAttribute(const QString &name) const
{
    QtPropertyAttributes::const_iterator it = attributes_.constFind(name);
    if(it != attributes_.constEnd())
    {
        return *it;
    }
    return QVariant();
}

void QtProperty::addChild(QtProperty *child)
{
    assert(child->getParent() == NULL);
    children_.push_back(child);
    child->parent_ = this;

    onChildAdd(child);
    emit signalPropertyInserted(child, this);
}

void QtProperty::removeChild(QtProperty *child)
{
    assert(child->getParent() == this);
    QtPropertyList::iterator it = std::find(children_.begin(), children_.end(), child);
    if(it != children_.end())
    {
        child->parent_ = NULL;
        children_.erase(it);

        onChildRemove(child);
        emit signalPropertyRemoved(child, this);
    }
}

void QtProperty::removeFromParent()
{
    if(parent_ != NULL)
    {
        parent_->removeChild(this);
    }
}

void QtProperty::removeAllChildren()
{
    QtPropertyList temp = children_;
    foreach(QtProperty *child, temp)
    {
        removeChild(child);
    }
}

int QtProperty::indexChild(const QtProperty *child) const
{
    return children_.indexOf(const_cast<QtProperty*>(child));
}

QtProperty* QtProperty::findChild(const QString &name)
{
    foreach(QtProperty *child, children_)
    {
        if(child->getName() == name)
        {
            return child;
        }
    }
    return NULL;
}

void QtProperty::onChildAdd(QtProperty *child)
{

}

void QtProperty::onChildRemove(QtProperty *child)
{

}

/********************************************************************/
QtGroupProperty::QtGroupProperty(int type, QObject *parent)
    : QtProperty(type, parent)
{

}

void QtGroupProperty::onChildAdd(QtProperty *child)
{
    connect(child, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotChildValueChange(QtProperty*)));
}

void QtGroupProperty::onChildRemove(QtProperty *child)
{
    disconnect(child, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotChildValueChange(QtProperty*)));
}

/********************************************************************/
static void ensureSize(QVariantList &list, int size)
{
    while(list.size() < size)
    {
        list.push_back(QVariant());
    }
}

QtListProperty::QtListProperty(int type, QObject *parent)
    : QtGroupProperty(type, parent)
{

}

void QtListProperty::setValue(const QVariant &value)
{
    if(value_ == value)
    {
        return;
    }
    value_ = value;

    QVariantList valueList = value.toList();
    ensureSize(valueList, children_.size());
    for(int i = 0; i < children_.size(); ++i)
    {
        children_[i]->setValue(valueList[i]);
    }

    emit signalValueChange(this);
}

void QtListProperty::slotChildValueChange(QtProperty *child)
{
    int i = indexChild(child);
    if(i >= 0)
    {
        QVariantList valueList = value_.toList();
        ensureSize(valueList, i + 1);

        if(valueList[i] != child->getValue())
        {
            valueList[i] = child->getValue();

            value_ = valueList;
            emit signalValueChange(this);
        }
    }
}

/********************************************************************/
QtDictProperty::QtDictProperty(int type, QObject *parent)
    : QtGroupProperty(type, parent)
{

}

void QtDictProperty::setValue(const QVariant &value)
{
    if(value_ == value)
    {
        return;
    }
    value_ = value;

    QVariantMap valueMap = value_.toMap();
    foreach (QtProperty *child, children_)
    {
        QVariant value = valueMap.value(child->getName());
        child->setValue(value);
    }

    emit signalValueChange(this);
}

void QtDictProperty::slotChildValueChange(QtProperty *property)
{
    QVariantMap valueMap = value_.toMap();
    QVariant oldValue = valueMap.value(property->getName());
    if(property->getValue() != oldValue)
    {
        valueMap[property->getName()] = property->getValue();

        value_ = valueMap;
        emit signalValueChange(this);
    }
}
