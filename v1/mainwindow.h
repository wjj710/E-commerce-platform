#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include "user.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     User *pr; //用户指针

private slots:
    void on_LoginButton_clicked();

    void on_RegisterButton_clicked();

    void on_addButton_clicked();

    void on_submitButton_clicked();

    void onTableClicked(const QModelIndex &index);

    void on_cancelButton_clicked();

    void on_deleteButton_clicked();

    void on_discountButton_clicked();

private:
    Ui::MainWindow *ui;
    int state; //登录状态
    QSqlTableModel *model; //商品数据模型
};

#endif // MAINWINDOW_H
