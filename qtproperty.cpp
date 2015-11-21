#include "qtproperty.h"
#include "qtpropertymanager.h"
#include <cassert>
#include <algorithm>

QtProperty::QtProperty(int type, QObject *parent)
    : QObject(parent)
{

}

void QtProperty::setName(const QString &name)
{
    name_ = name;
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
        emit signalValueChange();
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

QtPropertyAttributes& QtProperty::getAttributes()
{
    return attributes_;
}

void QtProperty::addChild(QtProperty *child)
{
    assert(child->getParent() == NULL);
    children_.push_back(child);
    child->parent_ = this;
    child->manager_ = manager_;
}

void QtProperty::removeChild(QtProperty *child)
{
    assert(child->getParent() == this);
    QtPropertyList::iterator it = std::find(children_.begin(), children_.end(), child);
    if(it != children_.end())
    {
        child->parent_ = NULL;
        child->manager_ = NULL;
        children_.erase(it);
    }
}

void QtProperty::removeFromParent()
{
    if(parent_ != NULL)
    {
        parent_->removeChild(this);
    }
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

QtPropertyList& QtProperty::getChildren()
{
    return children_;
}

void QtProperty::setManager(QtPropertyManager *manager)
{
    manager_ = manager;
}

void QtProperty::notifyPropertyChange()
{
    if(parent_ == NULL)
    {
        if(manager_ != NULL)
        {
            emit manager_->signalPropertyChange(this);
        }
    }
    else
    {
        parent_->onChildPopertyChange(this);
    }
}

void QtProperty::onChildPopertyChange(QtProperty *property)
{

}

void QtProperty::setChildPropertyValue(const QString &name, const QVariant &value)
{

}
