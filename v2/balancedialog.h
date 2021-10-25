#ifndef BALANCEDIALOG_H
#define BALANCEDIALOG_H

#include <QDialog>

namespace Ui {
class BalanceDialog;
}

class BalanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BalanceDialog(QWidget *parent = nullptr, double b=0);
    ~BalanceDialog();
    QString getBalance();

private:
    Ui::BalanceDialog *ui;
};

#endif // BALANCEDIALOG_H
