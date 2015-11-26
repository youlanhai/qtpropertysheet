#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qttreepropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertymanager.h"
#include "qtpropertyeditorfactory.h"

#include <QTreeWidget>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if 0
    QHBoxLayout *layout = new QHBoxLayout();

    QTreeWidget *widget = new QTreeWidget(ui->centralWidget);
    widget->setColumnCount(2);
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "name");
    item->setText(1, "value");
    widget->addTopLevelItem(item);

    layout->addWidget(widget);
    ui->centralWidget->setLayout(layout);

#else

    QtPropertyManager *manager = new QtPropertyManager(this);

    QtTreePropertyBrowser *browser = new QtTreePropertyBrowser(this);
    browser->init(ui->centralWidget);
    browser->setEditorFactory(new QtPropertyEditorFactory(this));

    {
        QtProperty *property = manager->createProperty(QtProperty::TYPE_STRING, manager);
        property->setName("test");
        property->setValue(QString("test value"));
        browser->addProperty(property);

        QtProperty *property2 = manager->createProperty(QtProperty::TYPE_STRING, manager);
        property2->setName("test2");
        property->addChild(property2);

        property2->setValue(QString("test value2"));
    }
    {
        QtProperty *rect = manager->createProperty(QtProperty::TYPE_LIST, manager);
        rect->setName("list");
        connect(rect, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onValueChanged(QtProperty*)));

        QtProperty *x = manager->createProperty(QtProperty::TYPE_DOUBLE, manager);
        x->setName("x");
        rect->addChild(x);

        QtProperty *y = manager->createProperty(QtProperty::TYPE_DOUBLE, manager);
        y->setName("y");
        rect->addChild(y);

        QtProperty *width = manager->createProperty(QtProperty::TYPE_DOUBLE, manager);
        width->setName("width");
        rect->addChild(width);

        QtProperty *height = manager->createProperty(QtProperty::TYPE_DOUBLE, manager);
        height->setName("height");
        rect->addChild(height);

        browser->addProperty(rect);

        QVariantList values;
        values.push_back(QVariant(8.0f));
        values.push_back(QVariant(9.0f));
        values.push_back(QVariant(200.0f));
        values.push_back(QVariant(100.0f));
        rect->setValue(values);
    }

#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onValueChanged(QtProperty *property)
{
    printf("property change: %s = %s\n",
           property->getName().toUtf8().data(),
           property->getValueString().toUtf8().data());

    fflush(stdout);
}
