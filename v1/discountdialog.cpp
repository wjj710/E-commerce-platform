#include "discountdialog.h"
#include "ui_discountdialog.h"

DiscountDialog::DiscountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiscountDialog)
{
    ui->setupUi(this);
    QStringList strlist;
    strlist<<"食物"<<"服装"<<"图书";
    ui->comboBox->addItems(strlist);
}

DiscountDialog::~DiscountDialog()
{
    delete ui;
}

int DiscountDialog::getType()
{
    return ui->comboBox->currentIndex();
}

int DiscountDialog::getNumber()
{
    return ui->spinBox->value();
}
