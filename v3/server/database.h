#ifndef FILE_H
#define FILE_H
#include <QFile>
#include <QMessageBox>
#include <QByteArray>
#include <QDebug>
#include "mainwindow.h"

void sendFile(MainWindow *m)
{
    QFile file;
    file.setFileName("database.db");
    //打开文件
    bool ok = file.open(QIODevice::ReadOnly);
    if(!ok){
        QMessageBox::critical(m,"错误","文件读取出错");
        return;
    }
    //开始发文件
    qint64 len=0;
    do{
        char buf[4*1024] = {0};
        len = file.read(buf, sizeof(buf));
        len = m->conn->write(buf, len);
    }while(len>0);
    file.close();
}

void receiveFile(MainWindow *m, QByteArray a, bool flag)
{
    QFile file;
    file.setFileName("database.db");
    //打开文件
    bool ok;
    if(!flag){
        ok = file.open(QIODevice::WriteOnly|QIODevice::Truncate);
    }else{
        ok = file.open(QIODevice::WriteOnly|QIODevice::Append);
    }
    if(!ok){
        QMessageBox::critical(m,"错误","文件读取出错");
        return;
    }
    //开始写文件
    int len=file.write(a);
    qDebug()<<len;
    file.close();
    m->conn->write("yes");
}

#endif // FILE_H
