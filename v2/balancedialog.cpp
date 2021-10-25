#include "balancedialog.h"
#include "ui_balancedialog.h"
#include <QDoubleValidator>

BalanceDialog::BalanceDialog(QWidget *parent, double b) :
    QDialog(parent),
    ui(new Ui::BalanceDialog)
{
    ui->setupUi(this);
    QDoubleValidator* adoubleValidator = new QDoubleValidator(0,10000,2);
    ui->lineEdit->setValidator(adoubleValidator);
    ui->balancelabel->setNum(b);
}

BalanceDialog::~BalanceDialog()
{
    delete ui;
}

QString BalanceDialog::getBalance(){
    return ui->lineEdit->text();
}
