#include "qtpropertytreedelegate.h"
#include "qtproperty.h"
#include "qtpropertytreeview.h"
#include "qttreepropertybrowser.h"

#include <QPainter>
#include <QFocusEvent>
#include <QApplication>

QtPropertyTreeDelegate::QtPropertyTreeDelegate(QObject *parent)
    : QItemDelegate(parent)
    , m_editorPrivate(0)
    , m_editedItem(0)
    , m_editedWidget(0)
    , m_disablePainting(false)
{}


int QtPropertyTreeDelegate::indentation(const QModelIndex &index) const
{
    if (!m_editorPrivate)
    {
        return 0;
    }

    QtPropertyTreeView *treeWidget = m_editorPrivate->treeWidget();

    QTreeWidgetItem *item = treeWidget->indexToItem(index);
    int indent = 0;
    while (item->parent())
    {
        item = item->parent();
        ++indent;
    }
    if (treeWidget->rootIsDecorated())
    {
        ++indent;
    }
    return indent * treeWidget->indentation();
}

void QtPropertyTreeDelegate::slotEditorDestroyed(QObject *object)
{
    if (QWidget *w = qobject_cast<QWidget *>(object))
    {
        const EditorToPropertyMap::iterator it = m_editorToProperty.find(w);
        if (it != m_editorToProperty.end())
        {
            m_propertyToEditor.remove(it.value());
            m_editorToProperty.erase(it);
        }

        if (m_editedWidget == w)
        {
            m_editedWidget = 0;
            m_editedItem = 0;
        }
    }
}

void QtPropertyTreeDelegate::closeEditor(QtProperty *property)
{
    if (QWidget *w = m_propertyToEditor.value(property, 0))
    {
        w->deleteLater();
    }
}

QWidget *QtPropertyTreeDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (index.column() == 1 && m_editorPrivate)
    {
        QTreeWidgetItem *item = m_editorPrivate->indexToItem(index);
        QtProperty *property = m_editorPrivate->indexToProperty(index);

        if (property && item && (item->flags() & Qt::ItemIsEnabled))
        {
            QWidget *editor = m_editorPrivate->createEditor(property, parent);
            if (editor)
            {
                editor->setAutoFillBackground(true);
                editor->installEventFilter(const_cast<QtPropertyTreeDelegate *>(this));
                connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)));

                m_propertyToEditor[property] = editor;
                m_editorToProperty[editor] = property;
                m_editedItem = item;
                m_editedWidget = editor;
            }
            return editor;
        }
    }
    return 0;
}

void QtPropertyTreeDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect.adjusted(0, 0, 0, -1));
}

void QtPropertyTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const
{
    bool hasValue = true;
    if (m_editorPrivate)
    {
        QtProperty *property = m_editorPrivate->indexToProperty(index);
        if (property)
        {
            hasValue = property->hasValue();
        }
    }
    QStyleOptionViewItemV3 opt = option;
    if ((m_editorPrivate && index.column() == 0) || !hasValue)
    {
        QtProperty *property = m_editorPrivate->indexToProperty(index);
        if (property && property->isModified())
        {
            opt.font.setBold(true);
            opt.fontMetrics = QFontMetrics(opt.font);
        }
    }
    QColor c;
    if (!hasValue && m_editorPrivate->markPropertiesWithoutValue())
    {
        c = opt.palette.color(QPalette::Dark);
        opt.palette.setColor(QPalette::Text, opt.palette.color(QPalette::BrightText));
    }
    else
    {
        c = QColor(180, 180, 180); //m_editorPrivate->calculatedBackgroundColor(m_editorPrivate->indexToBrowserItem(index));
        if (c.isValid() && (opt.features & QStyleOptionViewItemV2::Alternate))
            c = c.lighter(112);
    }
    if (c.isValid())
        painter->fillRect(option.rect, c);
    opt.state &= ~QStyle::State_HasFocus;
    if (index.column() == 1)
    {
        QTreeWidgetItem *item = m_editorPrivate->indexToItem(index);
        if (m_editedItem && m_editedItem == item)
            m_disablePainting = true;
    }

    QItemDelegate::paint(painter, opt, index);

    if (option.type)
    {
        m_disablePainting = false;
    }

    opt.palette.setCurrentColorGroup(QPalette::Active);
    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    if (!m_editorPrivate || (!m_editorPrivate->lastColumn(index.column()) && hasValue))
    {
        int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
        painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
    }
    painter->restore();
}

void QtPropertyTreeDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
            const QRect &rect, const QPixmap &pixmap) const
{
    if (m_disablePainting)
        return;

    QItemDelegate::drawDecoration(painter, option, rect, pixmap);
}

void QtPropertyTreeDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
            const QRect &rect, const QString &text) const
{
    if (m_disablePainting)
        return;

    QItemDelegate::drawDisplay(painter, option, rect, text);
}

QSize QtPropertyTreeDelegate::sizeHint(const QStyleOptionViewItem &option,
            const QModelIndex &index) const
{
    return QItemDelegate::sizeHint(option, index) + QSize(3, 4);
}

bool QtPropertyTreeDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusOut) {
        QFocusEvent *fe = static_cast<QFocusEvent *>(event);
        if (fe->reason() == Qt::ActiveWindowFocusReason)
            return false;
    }
    return QItemDelegate::eventFilter(object, event);
}
