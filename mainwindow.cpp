#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTreeWidget>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QHBoxLayout *layout = new QHBoxLayout();

    QTreeWidget *widget = new QTreeWidget(ui->centralWidget);
    widget->setColumnCount(2);
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, "name");
    item->setText(1, "value");
    widget->addTopLevelItem(item);

    layout->addWidget(widget);
    ui->centralWidget->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}
