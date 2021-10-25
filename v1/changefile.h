#ifndef CHANGEFILE_H
#define CHANGEFILE_H
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QStringList>
#include <QSqlRecord>
#include <QSqlError>
#include <QTextStream>
#include "mainwindow.h"

void changeFile(int index, int type, MainWindow *m, QString text){
    QString path;
    if(index==0){
        path="userdata/consumer.txt";
    }else{
        path="userdata/seller.txt";
    }
    //读取文件，将文件按行分开
    QFile readfile(path);
    if (!readfile.open(QIODevice::ReadOnly | QIODevice::Text)){
         QMessageBox::critical(m,"错误","文件读取出错");
         return;
    }
    QTextStream stream(&readfile);
    QString strall=stream.readAll();
    QStringList strlist=strall.split("\n"); //将文件流按行分开
    readfile.close();
    //重写文件
    QFile writefile(path);
    if (!writefile.open(QIODevice::WriteOnly | QIODevice::Text)){
         QMessageBox::critical(m,"错误","文件读取出错");
         return;
    }
    QTextStream stream1(&writefile);
    for(int i=0;i<strlist.count();i++){
        QString line=strlist.at(i);
        if(line.section(" ",0,0)==m->pr->account){
            if(type==1){
                stream1<<m->pr->account<<" "<<text<<" "<<line.section(" ",2,2)<<endl; //替换密码
            }else{
                stream1<<m->pr->account<<" "<<line.section(" ",1,1)<<" "<<text<<endl; //替换余额
            }
        }else{
            stream1<<line<<endl;
        }
    }
    writefile.close();
}

#endif // CHANGEFILE_H
