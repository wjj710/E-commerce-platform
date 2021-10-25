#include "mainwindow.h"
#include <QApplication>
#include <QSqlError>
#include <QDebug>
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!createConnection()) return 1;
    MainWindow w;
    w.show();
    return a.exec();
}
