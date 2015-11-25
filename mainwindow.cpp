#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qttreepropertybrowser.h"

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

    QtTreePropertyBrowser *browser = new QtTreePropertyBrowser(this);
    browser->init(ui->centralWidget);

#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}
