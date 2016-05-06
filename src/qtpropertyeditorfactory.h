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

    QtPropertyEditor* createPropertyEditor(QtProperty *property);

    QWidget* createEditor(QtProperty *property, QWidget *parent);

    void registerCreator(int type, QtPropertyEditorCreator method);

    template <typename T>
    void registerCreator(int type);

private:
    template <typename T>
    static QtPropertyEditor* internalCreator(QtProperty *property);

    typedef QMap<int, QtPropertyEditorCreator> CreatorMap;
    CreatorMap      creators_;
};

template <typename T>
void QtPropertyEditorFactory::registerCreator(int type)
{
    this->registerCreator(type, this->internalCreator<T>);
}

template <typename T>
QtPropertyEditor* QtPropertyEditorFactory::internalCreator(QtProperty *property)
{
    return new T(property);
}

#endif // QTPROPERTYEDITORFACTORY_H
