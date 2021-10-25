#include "infodialog.h"
#include "ui_infodialog.h"
#include <QPixmap>

InfoDialog::InfoDialog(QWidget *parent, QSqlRecord record) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    QPixmap photo;
    photo.loadFromData(record.value(8).toByteArray(),"JPG");
    ui->label->setPixmap(photo);
    ui->label->setScaledContents(true);
    ui->textBrowser->setText(record.value(3).toString());
    ui->name->setText(record.value(2).toString());
    ui->owner->setText(record.value(6).toString());
    ui->price->setText(record.value(4).toString());
    ui->remain->setText(record.value(5).toString());
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
