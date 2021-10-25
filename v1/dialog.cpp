#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QStringList strlist;
    strlist<<"食物"<<"服装"<<"图书";
    ui->comboBox->addItems(strlist);
    QIntValidator* aIntValidator = new QIntValidator(1,1000); //商品数量为整数，且不超过1000
    QDoubleValidator* adoubleValidator = new QDoubleValidator(0,10000,2); //商品价格为浮点数，不超过两位小数且数值不超过10000
    ui->numberlineEdit->setValidator(aIntValidator);
    ui->pricelineEdit->setValidator(adoubleValidator);
}

Dialog::~Dialog()
{
    delete ui;
}

int Dialog::getType(){
    return ui->comboBox->currentIndex();
}

QString Dialog::getName(){
    return ui->namelineEdit->text();
}

QString Dialog::getDescription(){
    return ui->textEdit->toPlainText();
}

double Dialog::getPrice(){
    return ui->pricelineEdit->text().toDouble();
}

int Dialog::getNumber(){
    return ui->numberlineEdit->text().toInt();
}



void Dialog::on_imageButton_clicked()
{
    path=QFileDialog::getOpenFileName(this,"打开文件","C:\\","*.jpg");
}
