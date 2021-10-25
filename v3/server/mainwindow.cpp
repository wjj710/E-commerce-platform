#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QByteArray>
#include <QString>
#include <QTime>
#include "database.h"
#include "file.h"

bool flag; //标记是否第一次收到数据包
bool connect1; //标记是否已连接
QTime curtime; //表示当前时间

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //构建TCP服务器
    listen = new QTcpServer(this);
    //创建监听
    listen->listen(QHostAddress("127.0.0.1"),9999);

    ui->label->setText("等待连接……");
    connect(listen, &QTcpServer::newConnection, [=](){
        if(!connect1){
           connect1=true;
           curtime=QTime::currentTime();
           ui->textEdit->append(QString("%1: 与客户端建立连接").arg(curtime.toString()));
        }else{
           return;
        }
        conn = listen->nextPendingConnection();
        sendFile(this);
        curtime=QTime::currentTime();
        ui->textEdit->append(QString("%1: 向客户端发送数据").arg(curtime.toString()));
        ui->label->setText("已连接");
        connect(conn, &QTcpSocket::readyRead, [=](){
            QByteArray array = conn->readAll();
            qDebug()<<array.size();
            if(array.size()>1000){ //大于1KB，说明是数据库文件
                qDebug()<<"receive";
                if(!flag){ //第一次收
                    flag=true;
                    curtime=QTime::currentTime();
                    ui->textEdit->append(QString("%1: 更新数据库").arg(curtime.toString()));
                    receiveFile(this, array,0);
                }else{
                    receiveFile(this, array,1);
                }
            }else{
                QString str = array;
                QString f = str.section(" ",0,0);
                int index = str.section(" ",1,1).toInt();
                QString a = str.section(" ",2,2);
                QString p = str.section(" ",3,3);
                int reply=2;
                double balance=0;
                if(f=="register"){ //注册
                    reply = register1(this,index,a,p);
                    curtime=QTime::currentTime();
                    ui->textEdit->append(QString("%1: 用户注册").arg(curtime.toString()));
                }else if(f=="login"){ //登录
                    reply = login(this,index,a,p,&balance);
                    curtime=QTime::currentTime();
                    ui->textEdit->append(QString("%1: 用户登录").arg(curtime.toString()));
                }else if(f=="balance"){ //改余额
                    int f1 = str.section(" ",4,4).toInt();
                    changeFile(index,2,this,a,p,f1);
                    curtime=QTime::currentTime();
                    ui->textEdit->append(QString("%1: 用户改余额").arg(curtime.toString()));
                }else if(f=="password"){ //改密码
                    int f1 = str.section(" ",4,4).toInt();
                    changeFile(index,1,this,a,p,f1);
                    curtime=QTime::currentTime();
                    ui->textEdit->append(QString("%1: 用户改密码").arg(curtime.toString()));
                }
                if(reply!=2){
                    conn->write((QString("%1 %2").arg(reply).arg(balance)).toUtf8());
                }
            }
        });

        connect(conn, &QTcpSocket::disconnected, [=](){
            ui->label->setText("等待连接……");
            curtime=QTime::currentTime();
            ui->textEdit->append(QString("%1: 与客户端断开连接").arg(curtime.toString()));
            flag=false;
            connect1=false;
        });
    });

}

MainWindow::~MainWindow()
{
    delete ui;
    delete listen;
    delete conn;
}

