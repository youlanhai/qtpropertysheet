#ifndef QTTREEPROPERTYBROWSER_H
#define QTTREEPROPERTYBROWSER_H

#include <QObject>
#include <QIcon>
#include <QMap>

class QWidget;
class QModelIndex;
class QTreeWidgetItem;
class QtPropertyTreeView;
class QtPropertyTreeDelegate;
class QtProperty;

class QtTreePropertyBrowser : public QObject
{
    Q_OBJECT
public:
    explicit QtTreePropertyBrowser(QObject *parent = 0);

    bool init(QWidget *parent);

    bool markPropertiesWithoutValue(){ return true; }
    bool lastColumn(int column);

    QWidget* createEditor(QtProperty *property, QWidget *parent);

    QTreeWidgetItem* editedItem();

    QTreeWidgetItem* indexToItem(const QModelIndex &index);
    QtProperty* indexToProperty(const QModelIndex &index);
    QtProperty* itemToProperty(QTreeWidgetItem* item);

    QtPropertyTreeView* treeWidget(){ return m_treeWidget; }

    void addProperty(QtProperty *property);
    void removeProperty(QtProperty *property);
    void removeAllProperties();

signals:

public slots:
    void slotCollapsed(const QModelIndex &);
    void slotExpanded(const QModelIndex &);
    void slotCurrentTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);

    //void slotPropertyInsert(QtProperty *property);
    //void slotPropertyRemove(QtProperty *property);

private:
    QtPropertyTreeView*         m_treeWidget;
    QtPropertyTreeDelegate*     m_delegate;
    QIcon       m_expandIcon;

    typedef QMap<QtProperty*, QTreeWidgetItem*> Property2ItemMap;
    Property2ItemMap            m_property2items;
};

#endif // QTTREEPROPERTYBROWSER_H
