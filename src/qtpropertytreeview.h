#ifndef QTPROPERTYTREEVIEW_H
#define QTPROPERTYTREEVIEW_H

#include <QTreeWidget>

class QtTreePropertyBrowser;

class QtPropertyTreeView : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QtPropertyTreeView(QWidget *parent = 0);

    void setEditorPrivate(QtTreePropertyBrowser *editorPrivate)
    {
        m_editorPrivate = editorPrivate;
    }

    QTreeWidgetItem* indexToItem(const QModelIndex &index)
    {
        return itemFromIndex(index);
    }

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QtTreePropertyBrowser *m_editorPrivate;
};

#endif // QTPROPERTYTREEVIEW_H
