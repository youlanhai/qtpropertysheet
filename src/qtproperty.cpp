#include "qtproperty.h"
#include "qtpropertymanager.h"
#include "qtpropertybrowserutils.h"
#include "qtattributename.h"

#include <QLocale>
#include <cassert>
#include <algorithm>

QtProperty::QtProperty(int type, QObject *parent)
    : QObject(parent)
    , type_(type)
    , parent_(NULL)
    , visible_(true)
    , selfVisible_(true)
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
    if(name != name_)
    {
        name_ = name;
        emit signalPropertyChange(this);
    }
}

const QString& QtProperty::getTitle() const
{
    return title_.isEmpty() ? name_ : title_;
}

void QtProperty::setTitle(const QString &title)
{
    if(title != title_)
    {
        title_ = title;
        emit signalPropertyChange(this);
    }
}

void QtProperty::setValue(const QVariant &value)
{
    if(value_ != value)
    {
        value_ = value;
        emit signalValueChange(this);
    }
}

void QtProperty::setVisible(bool visible)
{
    if(visible != visible_)
    {
        visible_ = visible;
        emit signalPropertyChange(this);
    }
}

QString QtProperty::getValueString() const
{
    return value_.toString();
}

QIcon QtProperty::getValueIcon() const
{
    return QIcon();
}

void QtProperty::setAttribute(const QString &name, const QVariant &value)
{
    attributes_[name] = value;

    emit signalAttributeChange(this, name);
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

void QtProperty::setChildValue(const QString &name, const QVariant &value)
{
    QtProperty *child = findChild(name);
    if(child != NULL)
    {
        child->setValue(value);
    }
}

void QtProperty::onChildAdd(QtProperty* /*child*/)
{

}

void QtProperty::onChildRemove(QtProperty* /*child*/)
{

}

/********************************************************************/
QtContainerProperty::QtContainerProperty(int type, QObject *parent)
    : QtProperty(type, parent)
{

}

void QtContainerProperty::onChildAdd(QtProperty *child)
{
    connect(child, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotChildValueChange(QtProperty*)));
}

void QtContainerProperty::onChildRemove(QtProperty *child)
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
    : QtContainerProperty(type, parent)
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

QString QtListProperty::getValueString() const
{
    QString text;
    text += "[";
    foreach(QtProperty *child, children_)
    {
        text += child->getValueString();
        text += ", ";
    }
    text += "]";
    return text;
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
    : QtContainerProperty(type, parent)
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

/********************************************************************/
QtGroupProperty::QtGroupProperty(int type, QObject *parent)
    : QtContainerProperty(type, parent)
{

}

void QtGroupProperty::setValue(const QVariant& /*value*/)
{

}

QtProperty* QtGroupProperty::findChild(const QString &name)
{
    QtProperty *result = NULL;
    foreach(QtProperty *child, children_)
    {
        if(name == child->getName())
        {
            result = child;
        }
        else if(child->getType() == TYPE_GROUP)
        {
            result = child->findChild(name);
        }

        if(result != NULL)
        {
            break;
        }
    }
    return result;
}

void QtGroupProperty::setChildValue(const QString &name, const QVariant &value)
{
    foreach(QtProperty *child, children_)
    {
        if(child->getType() == TYPE_GROUP)
        {
            child->setChildValue(name, value);
        }
        else if(name == child->getName())
        {
            child->setValue(value);
        }
    }
}

void QtGroupProperty::slotChildValueChange(QtProperty *property)
{
    // emit signal to listner directly.
    emit signalValueChange(property);
}


/********************************************************************/
QtEnumProperty::QtEnumProperty(int type, QObject *parent)
    : QtProperty(type, parent)
{

}

QString QtEnumProperty::getValueString() const
{
    int index = value_.toInt();
    QStringList enumNames = attributes_.value(QtAttributeName::EnumName).toStringList();
    if(index >= 0 && index < enumNames.size())
    {
        return enumNames[index];
    }
    return QString();
}

/********************************************************************/
QtFlagProperty::QtFlagProperty(int type, QObject *parent)
    : QtProperty(type, parent)
{

}

QString QtFlagProperty::getValueString() const
{
    int value = value_.toInt();
    QStringList enumNames = attributes_.value(QtAttributeName::FlagName).toStringList();

    QStringList selected;
    for(int i = 0; i < enumNames.size(); ++i)
    {
        if(value & (1 << i))
        {
            selected.push_back(enumNames[i]);
        }
    }
    return selected.join('|');
}

/********************************************************************/
QtBoolProperty::QtBoolProperty(int type, QObject *parent)
    : QtProperty(type, parent)
{

}

QString QtBoolProperty::getValueString() const
{
    return value_.toBool() ? "True" : "False";
}

QIcon QtBoolProperty::getValueIcon() const
{
    return QtPropertyBrowserUtils::drawCheckBox(value_.toBool());
}

/********************************************************************/
QtDoubleProperty::QtDoubleProperty(int type, QObject *parent)
    : QtProperty(type, parent)
{

}

QString QtDoubleProperty::getValueString() const
{
    QVariant v = getAttribute(QtAttributeName::Decimals);
    int decimals = v.type() == QVariant::Int ? v.toInt() : 2;
    return QLocale::system().toString(value_.toDouble(), 'f', decimals);
}

/********************************************************************/
QtColorProperty::QtColorProperty(int type, QObject *parent)
    : QtProperty(type, parent)
{

}

QString QtColorProperty::getValueString() const
{
    QColor color = value_.value<QColor>();
    return QtPropertyBrowserUtils::colorValueText(color);
}

QIcon QtColorProperty::getValueIcon() const
{
    QColor color = value_.value<QColor>();
    return QtPropertyBrowserUtils::brushValueIcon(QBrush(color));
}
