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
class QtPropertyEditorFactory;

typedef QMap<QtProperty*, QTreeWidgetItem*> Property2ItemMap;

class QtTreePropertyBrowser : public QObject
{
    Q_OBJECT
public:
    explicit QtTreePropertyBrowser(QObject *parent = 0);
    ~QtTreePropertyBrowser();

    bool init(QWidget *parent);

    void setEditorFactory(QtPropertyEditorFactory *factory);

    bool markPropertiesWithoutValue(){ return true; }
    bool lastColumn(int column);
    QColor calculatedBackgroundColor(QtProperty *property);

    QWidget* createEditor(QtProperty *property, QWidget *parent);

    QTreeWidgetItem* indexToItem(const QModelIndex &index);
    QtProperty* indexToProperty(const QModelIndex &index);
    QtProperty* itemToProperty(QTreeWidgetItem* item);

    QTreeWidgetItem* getEditedItem();
    QtPropertyTreeView* getTreeWidget(){ return treeWidget_; }

    void addProperty(QtProperty *property);
    void removeProperty(QtProperty *property);
    void removeAllProperties();
    Property2ItemMap& getProperties(){ return property2items_; }

    bool isExpanded(QtProperty *property);
    void setExpanded(QtProperty *property, bool expand);

public slots:
    void slotCurrentTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);

    void slotPropertyInsert(QtProperty *property, QtProperty *parent);
    void slotPropertyRemove(QtProperty *property, QtProperty *parent);
    void slotPropertyValueChange(QtProperty *property);
    void slotPropertyPropertyChange(QtProperty *property);

    void slotTreeViewDestroy(QObject *p);

private:
    void addProperty(QtProperty *property, QTreeWidgetItem *parentItem);
    void deleteTreeItem(QTreeWidgetItem *item);

    QtPropertyEditorFactory*    editorFactory_;
    QtPropertyTreeView*         treeWidget_;
    QtPropertyTreeDelegate*     delegate_;
    QIcon                       expandIcon_;

    Property2ItemMap            property2items_;
};

#endif // QTTREEPROPERTYBROWSER_H
