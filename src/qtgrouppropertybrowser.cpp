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
#include <QToolButton>

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

    QWidget*    titleBar_;
    QToolButton* titleButton_;
    QToolButton* titleMenu_;

    QWidget*    container_;
    QGridLayout* layout_;

    QtGroupItem* parent_;
    QList<QtGroupItem*> children_;
};

QtGroupItem::QtGroupItem()
    : property_(NULL)
    , editor_(NULL)
    , label_(NULL)
    , titleBar_(NULL)
    , titleButton_(NULL)
    , titleMenu_(NULL)
    , container_(NULL)
    , layout_(NULL)
    , parent_(NULL)
{

}

QtGroupItem::QtGroupItem(QtProperty *prop, QtGroupItem *parent, QtGroupPropertyBrowser *browser)
    : property_(prop)
    , editor_(NULL)
    , label_(NULL)
    , titleBar_(NULL)
    , titleButton_(NULL)
    , titleMenu_(NULL)
    , container_(NULL)
    , layout_(NULL)
    , parent_(parent)
{
    layout_ = parent->layout_;

    QtPropertyList &list = property_->getChildren();
    if(!list.empty())
    {
        titleBar_ = new QWidget();
        titleBar_->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum));
        titleBar_->setMinimumHeight(50);
        layout_->addWidget(titleBar_, layout_->rowCount(), 0, 1, 2);

        QHBoxLayout *titleLayout = new QHBoxLayout();
        titleLayout->setSpacing(0);
        titleLayout->setMargin(0);
        titleBar_->setLayout(titleLayout);

        titleButton_ = new QToolButton();
        titleButton_->setText(property_->getTitle());
        titleButton_->setCheckable(true);
        titleButton_->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
        titleButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        titleButton_->setArrowType(Qt::DownArrow);
        titleButton_->setIconSize(QSize(3, 16));
        titleLayout->addWidget(titleButton_);

        titleMenu_ = new QToolButton();
        titleMenu_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        titleMenu_->setText("menu");
        titleLayout->addWidget(titleMenu_);

        QFrame *frame2 = new QFrame();
        frame2->setFrameShape(QFrame::Panel);
        frame2->setFrameShadow(QFrame::Raised);
        container_ = frame2; //new QWidget();
        container_->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
        layout_->addWidget(container_, layout_->rowCount(), 0, 1, 2);

        layout_ = new QGridLayout();
        layout_->setSpacing(4);
        layout_->setMargin(4);
        container_->setLayout(layout_);
    }
    else
    {
        int row = layout_->rowCount();

        label_ = new QLabel(property_->getTitle());
        label_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        layout_->addWidget(label_, row, 0);

        editor_ = browser->createEditor(prop, NULL);
        if(editor_)
        {
            layout_->addWidget(editor_, row, 1);
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

    if(label_)
    {
        delete label_;
    }
    if(container_)
    {
        delete container_;
    }
    if(editor_)
    {
        delete editor_;
    }
}

void QtGroupItem::update()
{}

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
//    if(groupBox_)
//    {
//        groupBox_->setTitle(title);
//    }
    if(label_)
    {
        label_->setText(title);
    }
}

void QtGroupItem::setVisible(bool visible)
{
    if(container_)
    {
        container_->setVisible(visible);
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
    parentLayout->setMargin(4);
    parentLayout->setSpacing(0);
    parent->setLayout(parentLayout);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);

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
    removeAllProperties();
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
