#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    int getType();
    QString getName();
    QString getDescription();
    double getPrice();
    int getNumber();
    QString path;
private slots:
    void on_imageButton_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
