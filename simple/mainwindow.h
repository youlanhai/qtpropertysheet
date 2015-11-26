#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QtProperty;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onValueChanged(QtProperty *property);

private:
    Ui::MainWindow*     ui;
    QtProperty*         root_;
};

#endif // MAINWINDOW_H
