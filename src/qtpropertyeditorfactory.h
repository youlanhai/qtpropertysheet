#ifndef QTPROPERTYEDITORFACTORY_H
#define QTPROPERTYEDITORFACTORY_H

#include <QObject>
#include <QMap>

class QWidget;
class QtProperty;
class QtPropertyEditor;

typedef QtPropertyEditor* (*QtPropertyEditorCreator)(QtProperty *property);

class QtPropertyEditorFactory : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyEditorFactory(QObject *parent);

    QtPropertyEditor* createPropertyEditor(QtProperty *property, int type = 0);

    QWidget* createEditor(QtProperty *property, QWidget *parent);
    void registerCreator(int type, QtPropertyEditorCreator method);

private:
    typedef QMap<int, QtPropertyEditorCreator> CreatorMap;
    CreatorMap      creators_;
};

#endif // QTPROPERTYEDITORFACTORY_H
