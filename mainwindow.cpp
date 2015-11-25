#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qttreepropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertymanager.h"

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

    QtProperty *property = manager->addProperty(QtProperty::TYPE_STRING);
    property->setName("test");
    browser->addProperty(property);

    QtProperty *property2 = manager->addProperty(QtProperty::TYPE_STRING);
    property2->setName("test2");
    browser->addProperty(property2);

#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}
