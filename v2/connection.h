#ifndef CONNECTION_H
#define CONNECTION_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    if(!db.open()){
        QMessageBox::critical(0,"错误","文件读取出错");
        return false;
    }
    QSqlQuery query;
    query.exec(QString("create table goods (id int primary key, type int, name vchar, description vchar, price double, remain int, owner vchar, details vchar, picture image)"));
    query.exec(QString("create table shopping1 (id int primary key, name vchar, price double, number int, owner vchar, gid int)"));
    return true;
}

#endif // CONNECTION_H
