#ifndef DISCOUNTDIALOG_H
#define DISCOUNTDIALOG_H

#include <QDialog>

namespace Ui {
class DiscountDialog;
}

class DiscountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiscountDialog(QWidget *parent = nullptr);
    ~DiscountDialog();

    int getType();

    int getNumber();

private:
    Ui::DiscountDialog *ui;
};

#endif // DISCOUNTDIALOG_H
