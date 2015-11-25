#ifndef QTPROPERTYTREEEDITORDELEGATE_H
#define QTPROPERTYTREEEDITORDELEGATE_H

#include <QItemDelegate>

class QtProperty;
class QTreeWidgetItem;
class QtTreePropertyBrowser;

class QtPropertyTreeDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit QtPropertyTreeDelegate(QObject *parent = 0);

    void setEditorPrivate(QtTreePropertyBrowser *editorPrivate)
        { m_editorPrivate = editorPrivate; }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setModelData(QWidget *, QAbstractItemModel *,
            const QModelIndex &) const {}

    void setEditorData(QWidget *, const QModelIndex &) const {}

    bool eventFilter(QObject *object, QEvent *event);
    void closeEditor(QtProperty *property);

    QTreeWidgetItem *editedItem() const { return m_editedItem; }

protected:

    void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
            const QRect &rect, const QPixmap &pixmap) const;
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
            const QRect &rect, const QString &text) const;

private slots:
    void slotEditorDestroyed(QObject *object);

private:
    int indentation(const QModelIndex &index) const;

    typedef QMap<QWidget *, QtProperty *> EditorToPropertyMap;
    mutable EditorToPropertyMap m_editorToProperty;

    typedef QMap<QtProperty *, QWidget *> PropertyToEditorMap;
    mutable PropertyToEditorMap m_propertyToEditor;

    QtTreePropertyBrowser *     m_editorPrivate;
    mutable QTreeWidgetItem *   m_editedItem;
    mutable QWidget *           m_editedWidget;
    mutable bool                m_disablePainting;
};


#endif // QTPROPERTYTREEEDITORDELEGATE_H
