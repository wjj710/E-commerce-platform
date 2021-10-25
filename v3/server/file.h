#ifndef CHANGEFILE_H
#define CHANGEFILE_H
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QStringList>
#include <QTextStream>
#include "mainwindow.h"

void changeFile(int index, int type, MainWindow *m, QString account, QString text, int flag){
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
        if(line.section(" ",0,0)==account){
            if(type==1){
                stream1<<account<<" "<<text<<" "<<line.section(" ",2,2)<<endl; //替换密码
            }else{
                double k;
                if(flag==1){
                    k=text.toDouble()+line.section(" ",2,2).toDouble();
                }else{
                    k=line.section(" ",2,2).toDouble()-text.toDouble();
                }
                QString t=QString("%1").arg(k);
                stream1<<account<<" "<<line.section(" ",1,1)<<" "<<t<<endl; //替换余额
            }
        }else{
            stream1<<line<<endl;
        }
    }
    writefile.close();
    m->conn->write("success");
}

int login(MainWindow *m, int index, QString a, QString p, double *b)
{
    QString path;
    if(index==0){
        path="userdata/consumer.txt";
    }else{
        path="userdata/seller.txt";
    }
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
         QMessageBox::critical(m,"错误","文件读取出错");
         return 2;
    }
    //检查是否存在此账户
    QTextStream in(&file);
    QString l="",r="";
    double c=0;
    while(!in.atEnd()){
        QString line=in.readLine(0);
        l=line.section(" ",0,0);
        r=line.section(" ",1,1);
        c=line.section(" ",2,2).toDouble();
        if(l==a){
            break;
        }
    }
    if(l==a&&r==p){
        *b=c;
        return 1;
    }else{
        return 0;
    }
}

int register1(MainWindow *m, int index, QString a, QString p)
{
    QString path;
    if(index==0){
        path="userdata/consumer.txt";
    }else{
        path="userdata/seller.txt";
    }
    QFile file(path);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
         QMessageBox::critical(m,"错误","文件读取出错");
         return 2;
    }
    //检查账号是否被注册过
    QTextStream in(&file);
    QString l="";
    while(!in.atEnd()){
        QString line=in.readLine(0);
        int i=line.indexOf(" ");
        l=line.left(i);
        if(l==a){
            break;
        }
    }
    if(l==a){
        return 1;
    }
    if (!file.open(QIODevice::ReadWrite | QIODevice::Append)){
         QMessageBox::critical(m,"错误","文件读取出错");
         return 2;
    }
    //写入文件
    QTextStream out(&file);
    out<<a<<" "<<p<<" "<<"0"<<endl;
    out.flush();
    file.close();
    return 0;
}

#endif // CHANGEFILE_H

