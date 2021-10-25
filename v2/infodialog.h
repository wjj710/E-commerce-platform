#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QSqlRecord>
#include "mainwindow.h"

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = nullptr, QSqlRecord record = QSqlRecord(), int s=0, User *p=NULL, QSqlTableModel *mode1=NULL);
    ~InfoDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::InfoDialog *ui;
    QString name; //商品名称
    double price; //商品价格
    int remain;
    int state;
    User *pr;
    QSqlTableModel *model1;
    int id;
};

#endif // INFODIALOG_H
