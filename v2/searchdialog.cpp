#include "searchdialog.h"
#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
    QStringList strlist;
    strlist<<"食物"<<"服装"<<"图书"<<"全部";
    ui->comboBox->addItems(strlist);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

int SearchDialog::getType(){
    return ui->comboBox->currentIndex();
}

QString SearchDialog::getName(){
    return ui->lineEdit->text();
}
