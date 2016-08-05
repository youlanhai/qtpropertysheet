#include "qtgrouppropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertyeditorfactory.h"
#include "qtpropertybrowserutils.h"

#include <cassert>
#include <QTreeWidget>
#include <QApplication>
#include <QItemDelegate>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QHeaderView>
#include <QLineEdit>
#include <QLabel>

namespace
{
const int PropertyDataIndex = Qt::UserRole + 1;
}

class QtGroupItem
{
public:
    QtGroupItem();
    QtGroupItem(QtProperty *prop, QtGroupItem *parent, QtGroupPropertyBrowser *browser);
    virtual ~QtGroupItem();

    void update();
    void addChild(QtGroupItem *child);
    void removeChild(QtGroupItem *child);
    void removeFromParent();

    void setTitle(const QString &title);
    void setVisible(bool visible);

    QtGroupItem* parent(){ return parent_; }
    QtProperty* property(){ return property_; }

    void setLayout(QGridLayout *layout){ layout_ = layout; }

private:
    QtProperty* property_;
    QLabel*     label_;
    QWidget*    editor_; // can be null
    QLabel*     widgetLabel_;
    QGroupBox*  groupBox_;
    QGridLayout* layout_;
    QFrame*     line_;
    QtGroupItem* parent_;
    QList<QtGroupItem*> children_;
};

QtGroupItem::QtGroupItem()
    : property_(NULL)
    , editor_(NULL)
    , label_(NULL)
    , groupBox_(NULL)
    , layout_(NULL)
    , line_(NULL)
    , parent_(NULL)
{

}

QtGroupItem::QtGroupItem(QtProperty *prop, QtGroupItem *parent, QtGroupPropertyBrowser *browser)
    : property_(prop)
    , editor_(NULL)
    , label_(NULL)
    , groupBox_(NULL)
    , layout_(NULL)
    , line_(NULL)
    , parent_(parent)
{
    layout_ = parent->layout_;

    QtPropertyList &list = property_->getChildren();
    if(!list.empty())
    {
        groupBox_ = new QGroupBox(property_->getTitle());
        layout_->addWidget(groupBox_, layout_->rowCount(), 0, 1, 2);

        layout_ = new QGridLayout();
        groupBox_->setLayout(layout_);
    }
    else
    {
        int row = layout_->rowCount();

        label_ = new QLabel(property_->getTitle());
        layout_->addWidget(label_, row, 0, 1, 2);

        editor_ = browser->createEditor(prop, NULL);
        if(editor_)
        {
            layout_->addWidget(editor_, row, 1, 1, 2);
        }
    }
}

QtGroupItem::~QtGroupItem()
{
    removeFromParent();

    foreach(QtGroupItem *item, children_)
    {
        item->parent_ = NULL;
        delete item;
    }
}

void QtGroupItem::update()
{
    if(label_)
    {
        label_->setText(property_->getTitle());
        label_->setToolTip(property_->getToolTip());
    }
    if(groupBox_)
    {
        groupBox_->setTitle(property_->getTitle());
        groupBox_->setToolTip(property_->getToolTip());
        groupBox_->setVisible(property_->isVisible());
    }
}

void QtGroupItem::addChild(QtGroupItem *child)
{
    children_.push_back(child);
}

void QtGroupItem::removeChild(QtGroupItem *child)
{
    children_.removeOne(child);
}

void QtGroupItem::removeFromParent()
{
    if(parent_)
    {
        parent_->removeChild(this);
        parent_ = NULL;
    }
}

void QtGroupItem::setTitle(const QString &title)
{
    if(groupBox_)
    {
        groupBox_->setTitle(title);
    }
    if(label_)
    {
        label_->setText(title);
    }
}

void QtGroupItem::setVisible(bool visible)
{
    if(groupBox_)
    {
        groupBox_->setVisible(visible);
    }
    if(label_)
    {
        label_->setVisible(visible);
    }
    if(editor_)
    {
        editor_->setVisible(visible);
    }
}

QtGroupPropertyBrowser::QtGroupPropertyBrowser(QObject *parent)
    : QObject(parent)
    , editorFactory_(NULL)
    , rootItem_(NULL)
    , mainView_(NULL)
{

}

QtGroupPropertyBrowser::~QtGroupPropertyBrowser()
{
    removeAllProperties();

    if(rootItem_ != NULL)
    {
        delete rootItem_;
    }
}

bool QtGroupPropertyBrowser::init(QWidget *parent)
{
    QVBoxLayout *parentLayout = new QVBoxLayout();
    parent->setLayout(parentLayout);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->setMargin(0);

    QWidget *mainView_ = new QWidget(parent);
    mainView_->setLayout(mainLayout);
    parent->setFocusProxy(mainView_);
    parentLayout->addWidget(mainView_);

    QLayoutItem *item = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    parentLayout->addItem(item);

    rootItem_ = new QtGroupItem();
    rootItem_->setLayout(mainLayout);

    expandIcon_ = QtPropertyBrowserUtils::drawIndicatorIcon(mainView_->palette(), mainView_->style());

    connect(mainView_, SIGNAL(destroyed(QObject*)), this, SLOT(slotTreeViewDestroy(QObject*)));
    return true;
}

void QtGroupPropertyBrowser::setEditorFactory(QtPropertyEditorFactory *factory)
{
    editorFactory_ = factory;
}

bool QtGroupPropertyBrowser::lastColumn(int column)
{
    return false;
}

QColor QtGroupPropertyBrowser::calculatedBackgroundColor(QtProperty *property)
{
    if(dynamic_cast<QtGroupProperty*>(property) != 0)
    {
        return QColor(200, 200, 200);
    }
    return QColor(255, 255, 255);
}

QWidget* QtGroupPropertyBrowser::createEditor(QtProperty *property, QWidget *parent)
{
    if(editorFactory_ != NULL)
    {
        return editorFactory_->createEditor(property, parent);
    }
    return NULL;
}

QtGroupItem* QtGroupPropertyBrowser::getEditedItem()
{
    return NULL;
}

QtGroupItem* QtGroupPropertyBrowser::indexToItem(const QModelIndex &index)
{
    return NULL;
}

void QtGroupPropertyBrowser::slotCurrentTreeItemChanged(QtGroupItem*, QtGroupItem*)
{

}

QtProperty* QtGroupPropertyBrowser::indexToProperty(const QModelIndex &index)
{
    return NULL;
}

QtProperty* QtGroupPropertyBrowser::itemToProperty(QtGroupItem* item)
{
    return item->property();
}

void QtGroupPropertyBrowser::addProperty(QtProperty *property)
{
    if(property2items_.contains(property))
    {
        return;
    }

    addProperty(property, rootItem_);
}

void QtGroupPropertyBrowser::addProperty(QtProperty *property, QtGroupItem *parentItem)
{
    assert(parentItem != NULL);

    QtGroupItem *item = NULL;
    if(property->isSelfVisible())
    {
        item = new QtGroupItem(property, parentItem, this);
        parentItem->addChild(item);
        parentItem = item;
    }
    property2items_[property] = item;

    connect(property, SIGNAL(signalPropertyInserted(QtProperty*,QtProperty*)), this, SLOT(slotPropertyInsert(QtProperty*,QtProperty*)));
    connect(property, SIGNAL(signalPropertyRemoved(QtProperty*,QtProperty*)), this, SLOT(slotPropertyRemove(QtProperty*,QtProperty*)));
    connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotPropertyValueChange(QtProperty*)));
    connect(property, SIGNAL(signalPropertyChange(QtProperty*)), this, SLOT(slotPropertyPropertyChange(QtProperty*)));

    // add it's children finaly.
    foreach(QtProperty *child, property->getChildren())
    {
        addProperty(child, parentItem);
    }
}

void QtGroupPropertyBrowser::removeProperty(QtProperty *property)
{
    Property2ItemMap::iterator it = property2items_.find(property);
    if(it != property2items_.end())
    {
        QtGroupItem *item = it.value();
        if(item != NULL)
        {
            item->removeFromParent();
        }

        property2items_.erase(it);
        disconnect(property, 0, this, 0);

        // remove it's children first.
        foreach(QtProperty *child, property->getChildren())
        {
            removeProperty(child);
        }

        // then remove this QtGroupItem
        if(item != NULL)
        {
            deleteTreeItem(item);
        }
    }
}

void QtGroupPropertyBrowser::removeAllProperties()
{
    QList<QtProperty*> properties = property2items_.keys();
    foreach(QtProperty *property, properties)
    {
        removeProperty(property);
    }
    property2items_.clear();
}

void QtGroupPropertyBrowser::slotPropertyInsert(QtProperty *property, QtProperty *parent)
{
    QtGroupItem *parentItem = property2items_.value(parent);
    addProperty(property, parentItem);
}

void QtGroupPropertyBrowser::slotPropertyRemove(QtProperty *property, QtProperty * /*parent*/)
{
    removeProperty(property);
}

void QtGroupPropertyBrowser::slotPropertyValueChange(QtProperty *property)
{
//    QtGroupItem *item = property2items_.value(property);
//    if(item != NULL)
//    {
//        item->setText(1, property->getValueString());
//        item->setIcon(1, property->getValueIcon());
//    }
}

void QtGroupPropertyBrowser::slotPropertyPropertyChange(QtProperty *property)
{
    QtGroupItem *item = property2items_.value(property);
    if(item != NULL)
    {
        item->setTitle(property->getTitle());
        item->setVisible(property->isVisible());
    }
}

void QtGroupPropertyBrowser::slotTreeViewDestroy(QObject *p)
{
}

void QtGroupPropertyBrowser::deleteTreeItem(QtGroupItem *item)
{
    delete item;
}

bool QtGroupPropertyBrowser::isExpanded(QtProperty *property)
{
//    QtGroupItem *treeItem = property2items_.value(property);
//    if(treeItem != NULL)
//    {
//        return treeItem->isExpanded();
//    }
    return false;
}

void QtGroupPropertyBrowser::setExpanded(QtProperty *property, bool expand)
{
//    QtGroupItem *treeItem = property2items_.value(property);
//    if(treeItem != NULL)
//    {
//        treeItem->setExpanded(expand);
//    }
}
