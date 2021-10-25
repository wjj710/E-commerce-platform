#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QSqlRecord>

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = nullptr, QSqlRecord record = QSqlRecord());
    ~InfoDialog();

private:
    Ui::InfoDialog *ui;
};

#endif // INFODIALOG_H
