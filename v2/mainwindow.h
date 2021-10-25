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
    int state; //登录状态
    QSqlTableModel *model; //商品数据模型
    QSqlTableModel *model1; //购物车数据模型

private slots:
    void on_LoginButton_clicked();

    void on_RegisterButton_clicked();

    void on_addButton_clicked();

    void on_submitButton_clicked();

    void onTableClicked(const QModelIndex &index);

    void on_cancelButton_clicked();

    void on_deleteButton_clicked();

    void on_discountButton_clicked();

    void on_deleteButton1_clicked();

    void on_cancelButton1_clicked();

    void on_submitButton1_clicked();

    void on_orderButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
