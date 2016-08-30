#include "qtbuttonpropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertyeditorfactory.h"
#include "qtpropertybrowserutils.h"

#include <cassert>
#include <QApplication>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>

QtButtonItem::QtButtonItem()
    : property_(NULL)
    , label_(NULL)
    , editor_(NULL)
    , valueLabel_(NULL)
    , titleButton_(NULL)
    , titleMenu_(NULL)
    , container_(NULL)
    , layout_(NULL)
    , parent_(NULL)
    , bExpand_(true)
{

}

QtButtonItem::QtButtonItem(QtProperty *prop, QtButtonItem *parent, QtButtonPropertyBrowser *browser)
    : property_(prop)
    , label_(NULL)
    , editor_(NULL)
    , valueLabel_(NULL)
    , titleButton_(NULL)
    , titleMenu_(NULL)
    , container_(NULL)
    , layout_(NULL)
    , parent_(parent)
    , bExpand_(true)
{
    layout_ = parent->layout_;

    if(!property_->getChildren().empty())
    {
        int row = layout_->rowCount();

        titleButton_ = new QToolButton();
        titleButton_->setText(property_->getTitle());
        titleButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        titleButton_->setArrowType(Qt::UpArrow);
        titleButton_->setIconSize(QSize(16, 16));
        layout_->addWidget(titleButton_, row, 0);
        connect(titleButton_, SIGNAL(clicked(bool)), this, SLOT(onBtnExpand()));

        if(property_->isMenuVisible())
        {
            QFont font = titleButton_->font();
            font.setBold(true);
            font.setPointSize(16);
            titleButton_->setFont(font);
            titleButton_->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));

            titleMenu_ = new QToolButton();
            titleMenu_->setText("...");
            layout_->addWidget(titleMenu_, row, 1, Qt::AlignRight);
            connect(titleMenu_, SIGNAL(clicked(bool)), this, SLOT(onBtnMenu()));
        }
        else
        {
            QFont font = titleButton_->font();
            font.setBold(true);
            titleButton_->setFont(font);

            valueLabel_ = new QLabel(property_->getValueString());
            valueLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            layout_->addWidget(valueLabel_, row, 1, Qt::AlignLeft);
        }

        QFrame *frame2 = new QFrame();
        frame2->setFrameShape(QFrame::Panel);
        frame2->setFrameShadow(QFrame::Raised);
        container_ = frame2; //new QWidget();
        container_->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
        layout_->addWidget(container_, layout_->rowCount(), 0, 1, 2);

        layout_ = new QGridLayout();
        layout_->setSpacing(4);
        layout_->setMargin(8);
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
        else
        {
            valueLabel_ = new QLabel(property_->getValueString());
            valueLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            layout_->addWidget(valueLabel_, row, 1, Qt::AlignLeft);
        }
    }

    if(valueLabel_ != nullptr)
    {
        connect(property_, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onPropertyValueChange(QtProperty*)));
    }
}

QtButtonItem::~QtButtonItem()
{
    removeFromParent();

    foreach(QtButtonItem *item, children_)
    {
        item->parent_ = NULL;
        delete item;
    }

    if(titleButton_)
    {
        delete titleButton_;
    }
    if(titleMenu_)
    {
        delete titleMenu_;
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

void QtButtonItem::update()
{}

void QtButtonItem::addChild(QtButtonItem *child)
{
    children_.push_back(child);
}

void QtButtonItem::removeChild(QtButtonItem *child)
{
    children_.removeOne(child);
}

void QtButtonItem::removeFromParent()
{
    if(parent_)
    {
        parent_->removeChild(this);
        parent_ = NULL;
    }
}

void QtButtonItem::setTitle(const QString &title)
{
    if(titleButton_)
    {
        titleButton_->setText(title);
    }
    if(label_)
    {
        label_->setText(title);
    }
}

void QtButtonItem::setVisible(bool visible)
{
    if(titleButton_)
    {
        titleButton_->setVisible(visible);
    }
    if(titleMenu_)
    {
        titleMenu_->setVisible(visible);
    }
    if(container_)
    {
        container_->setVisible(visible && bExpand_);
    }
    if(label_)
    {
        label_->setVisible(visible);
    }
    if(editor_)
    {
        editor_->setVisible(visible);
    }
    if(valueLabel_)
    {
        valueLabel_->setVisible(visible);
    }
}

void QtButtonItem::setExpanded(bool expand)
{
    if(bExpand_ == expand)
    {
        return;
    }
    bExpand_ = expand;

    if(titleButton_)
    {
        titleButton_->setArrowType(expand ? Qt::UpArrow : Qt::DownArrow);
    }

    if(container_)
    {
        container_->setVisible(expand);
    }
}

void QtButtonItem::onBtnExpand()
{
    setExpanded(!bExpand_);
}

void QtButtonItem::onBtnMenu()
{
    emit property_->signalPopupMenu(property_);
}

void QtButtonItem::onPropertyValueChange(QtProperty *property)
{
    QString text = property_->getValueString();
    if(text.size() > 20)
    {
        text.remove(20, text.size() - 20);
        text += "...";
    }
    valueLabel_->setText(text);
}

QtButtonPropertyBrowser::QtButtonPropertyBrowser(QObject *parent)
    : QObject(parent)
    , editorFactory_(NULL)
    , rootItem_(NULL)
    , mainView_(NULL)
{

}

QtButtonPropertyBrowser::~QtButtonPropertyBrowser()
{
    removeAllProperties();

    if(rootItem_ != NULL)
    {
        delete rootItem_;
    }
}

bool QtButtonPropertyBrowser::init(QWidget *parent)
{
    QVBoxLayout *parentLayout = new QVBoxLayout();
    parentLayout->setMargin(4);
    parentLayout->setSpacing(0);
    parent->setLayout(parentLayout);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(4);

    QWidget *mainView_ = new QWidget(parent);
    mainView_->setLayout(mainLayout);
    parent->setFocusProxy(mainView_);
    parentLayout->addWidget(mainView_);

    QLayoutItem *item = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    parentLayout->addItem(item);

    rootItem_ = new QtButtonItem();
    rootItem_->setLayout(mainLayout);

    connect(mainView_, SIGNAL(destroyed(QObject*)), this, SLOT(slotViewDestroy(QObject*)));
    return true;
}

void QtButtonPropertyBrowser::setEditorFactory(QtPropertyEditorFactory *factory)
{
    editorFactory_ = factory;
}

QWidget* QtButtonPropertyBrowser::createEditor(QtProperty *property, QWidget *parent)
{
    if(editorFactory_ != NULL)
    {
        return editorFactory_->createEditor(property, parent);
    }
    return NULL;
}

QtProperty* QtButtonPropertyBrowser::itemToProperty(QtButtonItem* item)
{
    return item->property();
}

void QtButtonPropertyBrowser::addProperty(QtProperty *property)
{
    if(property2items_.contains(property))
    {
        return;
    }

    addProperty(property, rootItem_);
}

void QtButtonPropertyBrowser::addProperty(QtProperty *property, QtButtonItem *parentItem)
{
    assert(parentItem != NULL);

    QtButtonItem *item = NULL;
    if(property->isSelfVisible())
    {
        item = new QtButtonItem(property, parentItem, this);
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

void QtButtonPropertyBrowser::removeProperty(QtProperty *property)
{
    Property2ItemMap::iterator it = property2items_.find(property);
    if(it != property2items_.end())
    {
        QtButtonItem *item = it.value();
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

        // then remove this QtButtonItem
        if(item != NULL)
        {
            deleteItem(item);
        }
    }
}

void QtButtonPropertyBrowser::removeAllProperties()
{
    QList<QtProperty*> properties = property2items_.keys();
    foreach(QtProperty *property, properties)
    {
        removeProperty(property);
    }
    property2items_.clear();
}

void QtButtonPropertyBrowser::slotPropertyInsert(QtProperty *property, QtProperty *parent)
{
    QtButtonItem *parentItem = property2items_.value(parent);
    addProperty(property, parentItem);
}

void QtButtonPropertyBrowser::slotPropertyRemove(QtProperty *property, QtProperty * /*parent*/)
{
    removeProperty(property);
}

void QtButtonPropertyBrowser::slotPropertyValueChange(QtProperty *property)
{
//    QtButtonItem *item = property2items_.value(property);
//    if(item != NULL)
//    {
//        item->setText(1, property->getValueString());
//        item->setIcon(1, property->getValueIcon());
//    }
}

void QtButtonPropertyBrowser::slotPropertyPropertyChange(QtProperty *property)
{
    QtButtonItem *item = property2items_.value(property);
    if(item != NULL)
    {
        item->setTitle(property->getTitle());
        item->setVisible(property->isVisible());
    }
}

void QtButtonPropertyBrowser::slotViewDestroy(QObject *p)
{
    removeAllProperties();
}

void QtButtonPropertyBrowser::deleteItem(QtButtonItem *item)
{
    delete item;
}

bool QtButtonPropertyBrowser::isExpanded(QtProperty *property)
{
    QtButtonItem *item = property2items_.value(property);
    if(item != NULL)
    {
        return item->isExpanded();
    }
    return false;
}

void QtButtonPropertyBrowser::setExpanded(QtProperty *property, bool expand)
{
    QtButtonItem *item = property2items_.value(property);
    if(item != NULL)
    {
        item->setExpanded(expand);
    }
}
