#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qttreepropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertyfactory.h"
#include "qtpropertyeditorfactory.h"
#include "qtattributename.h"

#include <QTreeWidget>
#include <QHBoxLayout>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    root_(NULL)
{
    ui->setupUi(this);

    QtPropertyFactory *manager = new QtPropertyFactory(this);

    QtTreePropertyBrowser *browser = new QtTreePropertyBrowser(this);
    browser->init(ui->centralWidget);
    browser->setEditorFactory(new QtPropertyEditorFactory(this));

    QtProperty *root = manager->createProperty(QtProperty::TYPE_GROUP);
    root->setName("root");
    root->setSelfVisible(false);
    root_ = root;

    {
        QtProperty *group = manager->createProperty(QtProperty::TYPE_GROUP);
        group->setTitle("information");

        QtProperty *property = manager->createProperty(QtProperty::TYPE_STRING);
        property->setName("name");
        property->setValue(QString("no name"));
        group->addChild(property);

        QtProperty *property2 = manager->createProperty(QtProperty::TYPE_INT);
        property2->setName("age");
        group->addChild(property2);

        QtProperty *property3 = manager->createProperty(QtProperty::TYPE_DOUBLE);
        property3->setName("weight");
        property3->setValue(60.5);
        group->addChild(property3);

        QtProperty *property4 = manager->createProperty(QtProperty::TYPE_ENUM);
        property4->setName("country");
        QStringList countries;
        countries << "China" << "America" << "England";
        property4->setAttribute("enumNames", countries);
        group->addChild(property4);

        QtProperty *property5 = manager->createProperty(QtProperty::TYPE_BOOL);
        property5->setName("married");
        property5->setValue(true);
        group->addChild(property5);

        QtProperty *property6 = manager->createProperty(QtProperty::TYPE_COLOR);
        property6->setName("color");
        property6->setValue(QColor(255, 0, 0));
        group->addChild(property6);

        QtProperty *property7 = manager->createProperty(QtProperty::TYPE_FLAG);
        property7->setName("hobby");
        property7->setValue(1);
        QStringList hobby;
        hobby << "running" << "walking" << "swimming";
        property7->setAttribute("flagNames", hobby);
        group->addChild(property7);

        QtProperty *property8 = manager->createProperty(QtProperty::TYPE_FILE);
        property8->setName("head icon");
        property8->setValue(QString("no-image"));
        property8->setAttribute(QtAttributeName::FileDialogFilter, QString("Images(*.png *.jpg)"));
        property8->setAttribute(QtAttributeName::FileRelativePath, QDir::currentPath());
        group->addChild(property8);

        QtProperty *property9 = manager->createProperty(QtProperty::TYPE_ENUM_PAIR);
        property9->setName("sex");
        QStringList titles;
        titles << "unkown" << "man" << "woman";
        property9->setAttribute(QtAttributeName::EnumName, titles);
        property9->setAttribute(QtAttributeName::EnumValues, titles);
        property9->setValue("unkown");
        group->addChild(property9);

        root->addChild(group);
    }
    {
        QtProperty *hideRect = manager->createProperty(QtProperty::TYPE_BOOL);
        hideRect->setName("show geometry");
        hideRect->setValue(true);
        root->addChild(hideRect);

        QtProperty *rect = manager->createProperty(QtProperty::TYPE_LIST);
        rect->setName("geometry");

        QtProperty *x = manager->createProperty(QtProperty::TYPE_DOUBLE);
        x->setName("x");
        rect->addChild(x);

        QtProperty *y = manager->createProperty(QtProperty::TYPE_DOUBLE);
        y->setName("y");
        rect->addChild(y);

        QtProperty *width = manager->createProperty(QtProperty::TYPE_DOUBLE);
        width->setName("width");
        rect->addChild(width);

        QtProperty *height = manager->createProperty(QtProperty::TYPE_DOUBLE);
        height->setName("height");
        rect->addChild(height);

        root->addChild(rect);
    }
    {
        QtProperty *prop = manager->createProperty(QtProperty::TYPE_DYNAMIC_LIST);
        prop->setName("dynamic list");
        prop->setAttribute("valueType", QtProperty::TYPE_FILE);

        QVariantList values;
        values.push_back(QString("name"));
        values.push_back(QString("age"));
        prop->setValue(values);
        root->addChild(prop);
    }

    connect(root, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onValueChanged(QtProperty*)));
    browser->addProperty(root);


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


#if 0
    // test remove and add
    browser->removeProperty(root);
    browser->addProperty(root);
#endif

    //test delete
    // delete root

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

    if(property->getName() == "show geometry")
    {
        QtProperty *geometry = root_->findChild("geometry");
        if(geometry != NULL)
        {
            geometry->setVisible(property->getValue().toBool());
        }
    }
}
