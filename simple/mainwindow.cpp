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

    QtProperty *root = manager->createProperty(QtProperty::TYPE_GROUP, manager);
    root->setName("root");
    root->setSelfVisible(false);

    {
        QtProperty *group = manager->createProperty(QtProperty::TYPE_GROUP, manager);
        group->setTitle("information");

        QtProperty *property = manager->createProperty(QtProperty::TYPE_STRING, manager);
        property->setName("name");
        property->setValue(QString("no name"));
        group->addChild(property);

        QtProperty *property2 = manager->createProperty(QtProperty::TYPE_INT, manager);
        property2->setName("age");
        group->addChild(property2);

        QtProperty *property3 = manager->createProperty(QtProperty::TYPE_DOUBLE, manager);
        property3->setName("weight");
        property3->setValue(60.5);
        group->addChild(property3);

        QtProperty *property4 = manager->createProperty(QtProperty::TYPE_ENUM, manager);
        property4->setName("country");
        QStringList countries;
        countries << "China" << "America" << "England";
        property4->setAttribute("enumNames", countries);
        group->addChild(property4);

        root->addChild(group);
    }
    {
        QtProperty *rect = manager->createProperty(QtProperty::TYPE_LIST, manager);
        rect->setName("geometry");

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

        root->addChild(rect);
    }

    connect(root, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onValueChanged(QtProperty*)));
    browser->addProperty(root);

#if 1
    // test set property value

    //1. set child value by name.
    root->setChildValue("name", "Jack");

    //2. find property, then set value directly.
    QtProperty *addressProperty = root->findChild("age");
    if(addressProperty != NULL)
    {
        addressProperty->setValue(18);
    }

    //3. set list value
    QVariantList values;
    values.push_back(QVariant(8.0f));
    values.push_back(QVariant(9.0f));
    values.push_back(QVariant(200.0f));
    values.push_back(QVariant(100.0f));
    root->setChildValue("geometry", values);
#endif

#if 0
    // test remove and add
    browser->removeProperty(root);
    browser->addProperty(root);
#endif

    //test delete
    // delete root

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
