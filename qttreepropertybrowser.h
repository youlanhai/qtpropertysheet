#ifndef QTTREEPROPERTYBROWSER_H
#define QTTREEPROPERTYBROWSER_H

#include <QObject>
#include <QIcon>

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

signals:

public slots:
    void slotCollapsed(const QModelIndex &);
    void slotExpanded(const QModelIndex &);
    void slotCurrentTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);

private:
    QtPropertyTreeView*         m_treeWidget;
    QtPropertyTreeDelegate*     m_delegate;
    QIcon       m_expandIcon;
};

#endif // QTTREEPROPERTYBROWSER_H
