#include "infodialog.h"
#include "ui_infodialog.h"
#include <QPixmap>
#include <QInputDialog>
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QDebug>

extern bool bill;

InfoDialog::InfoDialog(QWidget *parent, QSqlRecord record ,int s, User *p, QSqlTableModel *m) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    QPixmap photo;
    photo.loadFromData(record.value(8).toByteArray(),"JPG");
    state=s;
    pr=p;
    model1=m;
    id=record.value(0).toInt();
    ui->label->setPixmap(photo);
    ui->label->setScaledContents(true);
    ui->textBrowser->setText(record.value(3).toString());
    ui->name->setText(record.value(2).toString());
    name=record.value(2).toString();
    ui->owner->setText(record.value(6).toString());
    ui->price->setText(record.value(4).toString());
    price=record.value(4).toDouble();
    ui->remain->setText(record.value(5).toString());
    remain=record.value(5).toInt();
    int index=record.value(1).toInt();
    QString text;
    if(index==0){
        text="食物";
    }else if(index==1){
        text="服装";
    }else{
        text="图书";
    }
    ui->type->setText(text);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}

void InfoDialog::on_pushButton_clicked()
{
    if(bill){
        QMessageBox::warning(this,"警告","您还有未支付的订单");
        return;
    }
    if(state==0){
        QMessageBox::warning(this,"警告","您还未登录");
    }else{
        if(pr->getUserType()==1){
            QMessageBox::warning(this,"警告","您无此权限");
        }else{
            bool ok;
            QString text = QInputDialog::getText(this, "加入购物车",
                                                       "数量:", QLineEdit::Normal,
                                                       "",&ok);
            if(ok){
                bool ok1;
                int k=text.toInt(&ok1);
                if(!ok1){
                    QMessageBox::warning(this,"警告","请输入正确的格式");
                }else{
                    if(k>remain){
                        QMessageBox::warning(this,"警告","商品剩余量不足");
                    }else{
                        QSqlRecord record=model1->record();
                        QString s = model1->filter();
                        model1->setFilter("");
                        model1->select();
                        int k1=model1->rowCount();
                        record.setValue(0,k1);
                        model1->setFilter(s);
                        model1->select();
                        record.setValue(1,name);
                        record.setValue(2,price);
                        record.setValue(3,k);
                        record.setValue(4,pr->account);
                        record.setValue(5,id);
                        model1->insertRecord(-1,record);
                        model1->submitAll();
                        QMessageBox::information(this,"提示","成功加入购物车");
                    }
                }
            }
        }
    }
}
