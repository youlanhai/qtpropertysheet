#include "qttreepropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertytreeview.h"
#include "qtpropertytreedelegate.h"

#include <cassert>
#include <QTreeWidget>
#include <QApplication>
#include <QItemDelegate>
#include <QHBoxLayout>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QHeaderView>

namespace
{

const int PropertyDataIndex = Qt::UserRole + 1;

bool isItemEditable(int flags)
{
    return (flags & Qt::ItemIsEditable) && (flags & Qt::ItemIsEnabled);
}


// Draw an icon indicating opened/closing branches
QIcon drawIndicatorIcon(const QPalette &palette, QStyle *style)
{
    QPixmap pix(14, 14);
    pix.fill(Qt::transparent);
    QStyleOption branchOption;
    QRect r(QPoint(0, 0), pix.size());
    branchOption.rect = QRect(2, 2, 9, 9); // ### hardcoded in qcommonstyle.cpp
    branchOption.palette = palette;
    branchOption.state = QStyle::State_Children;

    QPainter p;
    // Draw closed state
    p.begin(&pix);
    style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, &p);
    p.end();
    QIcon rc = pix;
    rc.addPixmap(pix, QIcon::Selected, QIcon::Off);
    // Draw opened state
    branchOption.state |= QStyle::State_Open;
    pix.fill(Qt::transparent);
    p.begin(&pix);
    style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, &p);
    p.end();

    rc.addPixmap(pix, QIcon::Normal, QIcon::On);
    rc.addPixmap(pix, QIcon::Selected, QIcon::On);
    return rc;
}

}





////

QtTreePropertyBrowser::QtTreePropertyBrowser(QObject *parent)
    : QObject(parent)
{

}

QtTreePropertyBrowser::~QtTreePropertyBrowser()
{
    removeAllProperties();
}

bool QtTreePropertyBrowser::init(QWidget *parent)
{
    QHBoxLayout *layout = new QHBoxLayout(parent);
    layout->setMargin(0);

    m_treeWidget = new QtPropertyTreeView(parent);
    m_treeWidget->setEditorPrivate(this);
    m_treeWidget->setIconSize(QSize(18, 18));
    layout->addWidget(m_treeWidget);
    parent->setFocusProxy(m_treeWidget);

    m_treeWidget->setColumnCount(2);
    QStringList labels;
    labels.append(QCoreApplication::translate("QtTreePropertyBrowser", "Property"));
    labels.append(QCoreApplication::translate("QtTreePropertyBrowser", "Value"));
    m_treeWidget->setHeaderLabels(labels);
    m_treeWidget->setAlternatingRowColors(true);
    m_treeWidget->setEditTriggers(QAbstractItemView::EditKeyPressed);

    m_delegate = new QtPropertyTreeDelegate(parent);
    m_delegate->setEditorPrivate(this);
    m_treeWidget->setItemDelegate(m_delegate);

    //m_treeWidget->header()->setMovable(false);
    //m_treeWidget->header()->setResizeMode(QHeaderView::Stretch);

    m_expandIcon = drawIndicatorIcon(m_treeWidget->palette(), m_treeWidget->style());

    connect(m_treeWidget, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(slotCollapsed(const QModelIndex &)));
    connect(m_treeWidget, SIGNAL(expanded(const QModelIndex &)), this, SLOT(slotExpanded(const QModelIndex &)));
    connect(m_treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(slotCurrentTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(m_treeWidget, SIGNAL(destroyed(QObject*)), this, SLOT(slotTreeViewDestroy(QObject*)));
    return true;
}

bool QtTreePropertyBrowser::lastColumn(int column)
{
    return m_treeWidget->header()->visualIndex(column) == m_treeWidget->columnCount() - 1;
}

QWidget* QtTreePropertyBrowser::createEditor(QtProperty *property, QWidget *parent)
{
    return 0;
}

QTreeWidgetItem* QtTreePropertyBrowser::editedItem()
{
    return m_delegate->editedItem();
}

QTreeWidgetItem* QtTreePropertyBrowser::indexToItem(const QModelIndex &index)
{
    return m_treeWidget->indexToItem(index);
}

void QtTreePropertyBrowser::slotCollapsed(const QModelIndex &)
{

}

void QtTreePropertyBrowser::slotExpanded(const QModelIndex &)
{

}

void QtTreePropertyBrowser::slotCurrentTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)
{

}

QtProperty* QtTreePropertyBrowser::indexToProperty(const QModelIndex &index)
{
    return itemToProperty(m_treeWidget->indexToItem(index));
}

QtProperty* QtTreePropertyBrowser::itemToProperty(QTreeWidgetItem* item)
{
    if(item != NULL)
    {
        quintptr ptr = item->data(0, PropertyDataIndex).value<quintptr>();
        return reinterpret_cast<QtProperty*>(ptr);
    }
    return NULL;
}

void QtTreePropertyBrowser::addProperty(QtProperty *property)
{
    if(m_property2items.contains(property))
    {
        return;
    }

    addProperty(property, NULL);
}

void QtTreePropertyBrowser::addProperty(QtProperty *property, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, property->getTitle());
    item->setData(0, PropertyDataIndex, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(property)));
    item->setText(1, property->getValueString());

    connect(property, SIGNAL(signalPropertyInserted(QtProperty*,QtProperty*)), this, SLOT(slotPropertyInsert(QtProperty*,QtProperty*)));
    connect(property, SIGNAL(signalPropertyRemoved(QtProperty*,QtProperty*)), this, SLOT(slotPropertyRemove(QtProperty*,QtProperty*)));
    connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotPropertyValueChange(QtProperty*)));
    m_property2items[property] = item;

    if(parentItem != NULL)
    {
        parentItem->addChild(item);
    }
    else
    {
        m_treeWidget->addTopLevelItem(item);
    }

    // add it's children finaly.
    foreach(QtProperty *child, property->getChildren())
    {
        addProperty(child, item);
    }
}

void QtTreePropertyBrowser::removeProperty(QtProperty *property)
{
    Property2ItemMap::iterator it = m_property2items.find(property);
    if(it != m_property2items.end())
    {
        QTreeWidgetItem *item = it.value();
        m_property2items.erase(it);
        disconnect(property, 0, this, 0);

        // remove it's children first.
        foreach(QtProperty *child, property->getChildren())
        {
            removeProperty(child);
        }

        // then remove this QTreeWidgetItem
        deleteTreeItem(item);
    }
}

void QtTreePropertyBrowser::removeAllProperties()
{
    QList<QtProperty*> properties = m_property2items.keys();
    foreach(QtProperty *property, properties)
    {
        removeProperty(property);
    }
    m_property2items.clear();
}

void QtTreePropertyBrowser::slotPropertyInsert(QtProperty *property, QtProperty *parent)
{
    QTreeWidgetItem *parentItem = m_property2items.value(parent);
    if(parentItem == NULL)
    {
        return;
    }

    addProperty(property, parentItem);
}

void QtTreePropertyBrowser::slotPropertyRemove(QtProperty *property, QtProperty *parent)
{
    removeProperty(property);
}

void QtTreePropertyBrowser::slotPropertyValueChange(QtProperty *property)
{
    QTreeWidgetItem *item = m_property2items.value(property);
    if(item != NULL)
    {
        item->setText(1, property->getValueString());
    }
}

void QtTreePropertyBrowser::slotTreeViewDestroy(QObject *p)
{
    if(m_treeWidget == p)
    {
        m_treeWidget = NULL;
    }
}

void QtTreePropertyBrowser::deleteTreeItem(QTreeWidgetItem *item)
{
    if(m_treeWidget)
    {
        delete item;
    }
}
