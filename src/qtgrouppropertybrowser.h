#ifndef QT_GROUP_PROPERTY_BROWSER_H
#define QT_GROUP_PROPERTY_BROWSER_H

#include <QObject>
#include <QIcon>
#include <QMap>

class QWidget;
class QLabel;
class QFrame;
class QGroupBox;
class QGridLayout;

class QtProperty;
class QtPropertyEditorFactory;
class QtGroupPropertyBrowser;

class QtGroupItem;

class QtGroupPropertyBrowser : public QObject
{
    Q_OBJECT
public:
    typedef QMap<QtProperty*, QtGroupItem*> Property2ItemMap;

    explicit QtGroupPropertyBrowser(QObject *parent = 0);
    ~QtGroupPropertyBrowser();

    bool init(QWidget *parent);

    void setEditorFactory(QtPropertyEditorFactory *factory);

    bool markPropertiesWithoutValue(){ return true; }
    bool lastColumn(int column);
    QColor calculatedBackgroundColor(QtProperty *property);

    QWidget* createEditor(QtProperty *property, QWidget *parent);

    QtGroupItem* indexToItem(const QModelIndex &index);
    QtProperty* indexToProperty(const QModelIndex &index);
    QtProperty* itemToProperty(QtGroupItem* item);

    QtGroupItem* getEditedItem();

    void addProperty(QtProperty *property);
    void removeProperty(QtProperty *property);
    void removeAllProperties();
    Property2ItemMap& getProperties(){ return property2items_; }

    bool isExpanded(QtProperty *property);
    void setExpanded(QtProperty *property, bool expand);

public slots:
    void slotCurrentTreeItemChanged(QtGroupItem*, QtGroupItem*);

    void slotPropertyInsert(QtProperty *property, QtProperty *parent);
    void slotPropertyRemove(QtProperty *property, QtProperty *parent);
    void slotPropertyValueChange(QtProperty *property);
    void slotPropertyPropertyChange(QtProperty *property);

    void slotTreeViewDestroy(QObject *p);

private:
    void addProperty(QtProperty *property, QtGroupItem *parentItem);
    void deleteTreeItem(QtGroupItem *item);

    QtPropertyEditorFactory*    editorFactory_;
    QIcon                       expandIcon_;

    QtGroupItem*                rootItem_;
    QWidget*                    mainView_;

    Property2ItemMap            property2items_;
};

#endif // QT_GROUP_PROPERTY_BROWSER_H
