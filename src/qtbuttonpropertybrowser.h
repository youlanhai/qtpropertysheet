#ifndef QT_BUTTON_PROPERTY_BROWSER_H
#define QT_BUTTON_PROPERTY_BROWSER_H

#include <QObject>
#include <QIcon>
#include <QMap>

class QWidget;
class QLabel;
class QFrame;
class QGroupBox;
class QGridLayout;
class QToolButton;

class QtProperty;
class QtPropertyEditorFactory;
class QtButtonPropertyBrowser;


class QtButtonItem : public QObject
{
    Q_OBJECT
public:
    QtButtonItem();
    QtButtonItem(QtProperty *prop, QtButtonItem *parent, QtButtonPropertyBrowser *browser);
    virtual ~QtButtonItem();

    void update();
    void addChild(QtButtonItem *child);
    void removeChild(QtButtonItem *child);
    void removeFromParent();

    void setTitle(const QString &title);
    void setVisible(bool visible);

    QtButtonItem* parent(){ return parent_; }
    QtProperty* property(){ return property_; }

    void setLayout(QGridLayout *layout){ layout_ = layout; }

    void setExpanded(bool expand);
    bool isExpanded() const{ return bExpand_; }

protected slots:
    void onBtnExpand();
    void onBtnMenu();

protected:
    QtProperty* property_;
    QLabel*     label_;
    QWidget*    editor_; // can be null

    QToolButton* titleButton_;
    QToolButton* titleMenu_;

    QWidget*     container_;
    QGridLayout* layout_;

    QtButtonItem* parent_;
    QList<QtButtonItem*> children_;

    bool        bExpand_;
};


class QtButtonPropertyBrowser : public QObject
{
    Q_OBJECT
public:
    typedef QMap<QtProperty*, QtButtonItem*> Property2ItemMap;

    explicit QtButtonPropertyBrowser(QObject *parent = 0);
    ~QtButtonPropertyBrowser();

    bool init(QWidget *parent);

    void setEditorFactory(QtPropertyEditorFactory *factory);
    QWidget* createEditor(QtProperty *property, QWidget *parent);

    QtProperty* itemToProperty(QtButtonItem* item);

    void addProperty(QtProperty *property);
    void removeProperty(QtProperty *property);
    void removeAllProperties();
    Property2ItemMap& getProperties(){ return property2items_; }

    bool isExpanded(QtProperty *property);
    void setExpanded(QtProperty *property, bool expand);

public slots:
    void slotPropertyInsert(QtProperty *property, QtProperty *parent);
    void slotPropertyRemove(QtProperty *property, QtProperty *parent);
    void slotPropertyValueChange(QtProperty *property);
    void slotPropertyPropertyChange(QtProperty *property);

    void slotViewDestroy(QObject *p);

private:
    void addProperty(QtProperty *property, QtButtonItem *parentItem);
    void deleteItem(QtButtonItem *item);

    QtPropertyEditorFactory*    editorFactory_;

    QtButtonItem*                rootItem_;
    QWidget*                    mainView_;

    Property2ItemMap            property2items_;
};

#endif // QT_BUTTON_PROPERTY_BROWSER_H
