#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "dialog.h"
#include "consumer.h"
#include "seller.h"
#include "balancedialog.h"
#include "infodialog.h"
#include "searchdialog.h"
#include "discountdialog.h"
#include <QPixmap>
#include <QPalette>
#include <QHeaderView>
#include <QByteArray>
#include <QButtonGroup>
#include <QCheckBox>
#include <QHostAddress>
#include <QFile>
#include <QMessageBox>
#include <QSqlError>
#include <QInputDialog>

void changeFile(int index, int type, MainWindow *m, QString account,QString text,int flag)
{
    QString s;
    if(type==2){
        s="balance";
    }else{
        s="password";
    }
    m->conn->write(QString("%1 %2 %3 %4 %5").arg(s).arg(index).arg(account).arg(text).arg(flag).toUtf8());
    m->loop.exec(QEventLoop::ExcludeUserInputEvents);
}

int k; //记录数据库中的商品数量
int k1; //记录数据库中的购物车记录数量
bool bill=false; //记录是否有未支付账单
double sum; //记录账单总金额
bool isconnect; //记录是否连接

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //设置初始状态为未登录、未连接
    state=0;
    ui->setupUi(this);
    ui->statusBar->showMessage("等待连接……");
    isconnect=false;
    this->setAttribute(Qt::WA_DeleteOnClose);

    //设置背景图片
    QPixmap pixmap = QPixmap(":/Images/background.jpg").scaled(this->size());
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette(palette);

    //开始时进入登录页面
    ui->stackedWidget->setCurrentIndex(0);

    //设置下拉框
    QStringList strlist;
    strlist<<"消费者"<<"商家";
    ui->comboBox->addItems(strlist);

    //与服务器连接
    conn = new QTcpSocket(this);
    conn->connectToHost(QHostAddress("127.0.0.1"),9999);

    connect(conn, SIGNAL(readyRead()), &loop, SLOT(quit()));
    connect(conn, SIGNAL(disconnected()), &loop, SLOT(quit()));

    //设置连接建立和断开的操作
    connect(conn, &QTcpSocket::connected, [=](){
        ui->statusBar->showMessage("已连接");
        isconnect=true;
    });

    connect(conn, &QTcpSocket::disconnected, [=](){
        ui->statusBar->showMessage("等待连接……");
        isconnect=false;
    });

    //设置收到信息的操作
    connect(conn, &QTcpSocket::readyRead, [=](){
        QByteArray array = conn->readAll();
        if(array.size()>1000){ //从服务器加载数据库
            qDebug()<<QString("%1").arg(array.size());
            QFile file;
            file.setFileName("database.db");
            //打开文件
            bool ok;
            ok = file.open(QIODevice::WriteOnly|QIODevice::Append);
            if(!ok){
                QMessageBox::critical(this,"错误","文件读取出错");
                return;
            }
            //开始写文件
            file.write(array);
            file.close();
            //打开数据库
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("database.db");
            if(!db.open()){
                QMessageBox::critical(0,"错误","文件读取出错");
                exit(0);
            }
            model = new QSqlTableModel(this);
            model1 = new QSqlTableModel(this);
            model->setTable("goods");
            model1->setTable("shopping1");
            model->select();
            model1->select();
            model->setHeaderData(1,Qt::Horizontal,"种类");
            model->setHeaderData(2,Qt::Horizontal,"名称");
            model->setHeaderData(3,Qt::Horizontal,"描述");
            model->setHeaderData(4,Qt::Horizontal,"价格");
            model->setHeaderData(5,Qt::Horizontal,"剩余量");
            model->setHeaderData(6,Qt::Horizontal,"商家");
            model1->setHeaderData(1,Qt::Horizontal,"名称");
            model1->setHeaderData(2,Qt::Horizontal,"价格");
            model1->setHeaderData(3,Qt::Horizontal,"数量");
        }else{
            str=array;
        }
    });

    //设置菜单栏槽函数
    connect(ui->actionlogin,&QAction::triggered,[=](){ //登录
        if(!isconnect){
            QMessageBox::warning(this,"警告","未与服务器连接");
            return;
        }
        if(state==0){
            ui->stackedWidget->setCurrentIndex(0);
        }else{
            QMessageBox::warning(this,"警告","您已登录");
        }
    });

    connect(ui->actionmanagement,&QAction::triggered,[=](){ //商品管理
        if(!isconnect){
            QMessageBox::warning(this,"警告","未与服务器连接");
            return;
        }
        if(state){
            if(pr->getUserType()==1){
                ui->stackedWidget->setCurrentIndex(1);
                QString owner=pr->account;
                model->setFilter(QString("owner = '%1'").arg(owner));
                model->select();
                model->setEditStrategy(QSqlTableModel::OnManualSubmit);
                ui->tableView_2->setModel(model);
                ui->tableView_2->setColumnHidden(0,1);
                ui->tableView_2->setColumnHidden(1,1);
                ui->tableView_2->setColumnHidden(3,1);
                ui->tableView_2->setColumnHidden(6,1);
                ui->tableView_2->setColumnHidden(7,1);
                ui->tableView_2->setColumnHidden(8,1);
                ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            }else{
                QMessageBox::warning(this,"警告","您无此权限");
            }
        }else{
            QMessageBox::warning(this,"警告","您还未登录");
        }
    });

    connect(ui->actioninformation,&QAction::triggered,[=](){ //商品信息
        if(!isconnect){
            QMessageBox::warning(this,"警告","未与服务器连接");
            return;
        }
        ui->stackedWidget->setCurrentIndex(2);
        model->setFilter("");
        model->select();
        ui->tableView->setModel(model); //显示商品信息
        ui->tableView->setColumnHidden(0,1);
        ui->tableView->setColumnHidden(1,1);
        ui->tableView->setColumnHidden(4,1);
        ui->tableView->setColumnHidden(5,1);
        ui->tableView->setColumnHidden(6,1);
        ui->tableView->setColumnHidden(8,1);
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置商品信息不可编辑
        ui->tableView->verticalHeader()->setVisible(false);
        ui->tableView->horizontalHeader()->setVisible(false);
    });

    connect(ui->actionlogout,&QAction::triggered,[=](){ //注销
        if(!isconnect){
            QMessageBox::warning(this,"警告","未与服务器连接");
            return;
        }
        if(state==0){
            QMessageBox::warning(this,"警告","您还未登录");
        }else{
            ui->stackedWidget->setCurrentIndex(0);
            ui->Account->clear();
            ui->Password->clear();
            state=0;
            delete pr;
        }
    });

    connect(ui->actionchange_password,&QAction::triggered,[=](){ //密码管理
        if(!isconnect){
            QMessageBox::warning(this,"警告","未与服务器连接");
            return;
        }
        if(state){
            bool ok;
            QString text = QInputDialog::getText(this, "修改密码",
                                                       "新密码:", QLineEdit::Normal,
                                                       "", &ok);
            if (ok){
                if(text!=""){
                    if(text.indexOf(" ")!=-1){
                        QMessageBox::warning(this,"警告","密码不能包含空格");
                    }else{
                        int index=pr->getUserType();
                        pr->password=text;
                        changeFile(index,1,this,pr->account,text,1);
                        QMessageBox::information(this,"提示","密码修改成功");
                    }
                }else{
                    QMessageBox::warning(this,"警告","密码不能为空");
                }
            }
        }else{
            QMessageBox::warning(this,"警告","您还未登录");
        }
    });

    connect(ui->actionbalance_management,&QAction::triggered,[=](){ //余额管理
        if(!isconnect){
            QMessageBox::warning(this,"警告","未与服务器连接");
            return;
        }
        if(state){
            BalanceDialog *mydialog = new BalanceDialog(this,pr->balance);
            Qt::WindowFlags flags=mydialog->windowFlags();
            mydialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint);//设置窗口为固定大小
            int ret=mydialog->exec () ;
            if (ret==BalanceDialog::Accepted){
                QString text=mydialog->getBalance();
                if(text!=""){
                    int index=pr->getUserType();
                    pr->balance+=text.toDouble();
                    changeFile(index,2,this,pr->account, text,1);
                    QMessageBox::information(this,"提示","充值成功");
                }
            }
        }else{
            QMessageBox::warning(this,"警告","您还未登录");
        }
    });

    connect(ui->actionsearch,&QAction::triggered,[=](){ //搜索
        if(!isconnect){
            QMessageBox::warning(this,"警告","未与服务器连接");
            return;
        }
        SearchDialog *dlg=new SearchDialog(this);
        Qt::WindowFlags flags=dlg->windowFlags();
        dlg->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint);//设置窗口为固定大小
        int ret=dlg->exec();
        if(ret==SearchDialog::Accepted){
            int type=dlg->getType();
            QString name=dlg->getName();
            if(type!=3&&name!=""){
                model->setFilter(QString("(type='%1')AND(name='%2')").arg(QString::number(type)).arg(name));
            }else if(name!=""){
                model->setFilter(QString("name='%1'").arg(name));
            }else if(type!=3){
                model->setFilter(QString("type='%1'").arg(QString::number(type)));
            }
            model->select();
            ui->stackedWidget->setCurrentIndex(2);
            ui->tableView->setModel(model); //显示商品信息
            ui->tableView->setColumnHidden(0,1);
            ui->tableView->setColumnHidden(1,1);
            ui->tableView->setColumnHidden(4,1);
            ui->tableView->setColumnHidden(5,1);
            ui->tableView->setColumnHidden(6,1);
            ui->tableView->setColumnHidden(8,1);
            ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置商品信息不可编辑
            ui->tableView->verticalHeader()->setVisible(false);
            ui->tableView->horizontalHeader()->setVisible(false);
        }
        delete dlg;
    });

    connect(ui->actionshopping,&QAction::triggered,[=](){ //购物车
        if(!isconnect){
            QMessageBox::warning(this,"警告","未与服务器连接");
            return;
        }
        if(state==0){
            QMessageBox::warning(this,"警告","您还未登录");
        }else{
            if(pr->getUserType()==1){
                QMessageBox::warning(this,"警告","您无此权限");
            }else{
                ui->stackedWidget->setCurrentIndex(4);
                model1->setFilter(QString("owner='%1'").arg(pr->account));
                model1->select();
                model->setEditStrategy(QSqlTableModel::OnManualSubmit);
                ui->tableView_3->setModel(model1);
                ui->tableView_3->setColumnHidden(0,1);
                ui->tableView_3->setColumnHidden(4,1);
                ui->tableView_3->setColumnHidden(5,1);
                ui->tableView_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                if(bill){
                    ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置订单信息不可编辑
                }
            }
        }
    });

    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete pr;
}

void MainWindow::on_LoginButton_clicked()
{
    QString a=ui->Account->text(),p=ui->Password->text();
    if(a!=""&&p!=""){
        int index=ui->comboBox->currentIndex();
        //向服务器发信息
        conn->write(QString("%1 %2 %3 %4").arg("login").arg(index).arg(a).arg(p).toUtf8());
        //进入事件循环等待服务器回应
        loop.exec(QEventLoop::ExcludeUserInputEvents);
        //接到服务器回应
        int reply = str.section(" ",0,0).toInt();
        double balance = str.section(" ",1,1).toDouble();
        if(reply){
            ui->stackedWidget->setCurrentIndex(3);
            ui->label_4->setText(a);
            QPalette pe;
            pe.setColor(QPalette::WindowText,Qt::white);
            ui->label_3->setPalette(pe);
            ui->label_4->setPalette(pe);
            state=1;
            if(index==0){
                pr=new Consumer(a,p,balance,0);
            }else{
                pr=new Seller(a,p,balance,1);
            }
        }else{
            QMessageBox::warning(this,"警告","账号或密码错误");
        }
    }else{
        QMessageBox::warning(this,"警告","账号和密码不能为空");
    }
}

void MainWindow::on_RegisterButton_clicked()
{
    QString a=ui->Account->text(),p=ui->Password->text();
    if(a!=""&&p!=""){
        int i=a.indexOf(" "),j=p.indexOf(" ");
        if(i!=-1||j!=-1){
            QMessageBox::warning(this,"警告","账号和密码不能包含空格");
            return;
        }
        int index=ui->comboBox->currentIndex();
        //向服务器发信息
        conn->write(QString("%1 %2 %3 %4").arg("register").arg(index).arg(a).arg(p).toUtf8());
        //进入事件循环等待服务器回应
        loop.exec(QEventLoop::ExcludeUserInputEvents);
        //接到服务器回应
        int reply = str.section(" ",0,0).toInt();
        if(!reply){
            QMessageBox::information(this,"提示","注册成功");
        }else{
            QMessageBox::warning(this,"警告","此账号已被注册");
        }
    }else{
        QMessageBox::warning(this,"警告","账号和密码不能为空");
    }
}

void MainWindow::on_addButton_clicked()
{
    Dialog *mydialog = new Dialog(this);
    Qt::WindowFlags flags=mydialog->windowFlags();
    mydialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint);//设置窗口为固定大小
    int ret=mydialog->exec () ;
    if(ret==QDialog::Accepted){
        QSqlRecord record=model->record();
        //更新k的值（即数据库中商品的数量）
        QString s = model->filter();
        model->setFilter("");
        model->select();
        k=model->rowCount();
        model->setFilter(s);
        model->select();
        //设置要插入的记录中各项参数的值
        record.setValue(0,k);
        k++; //模型中商品数量加一
        record.setValue(1,mydialog->getType());
        record.setValue(2,mydialog->getName());
        record.setValue(3,mydialog->getDescription());
        record.setValue(4,mydialog->getPrice());
        record.setValue(5,mydialog->getNumber());
        record.setValue(6,pr->account);
        record.setValue(7,"详情");
        QFile *file= new QFile(mydialog->path);
        file->open(QIODevice::ReadOnly);
        QByteArray data = file->readAll();
        file->close();
        QVariant var(data);
        record.setValue(8,var);
        model->insertRecord(-1,record);
        model->submitAll();
    }
    delete mydialog;
}

void MainWindow::on_submitButton_clicked()
{
    int ok = QMessageBox::warning(this,tr("警告"),tr("确定提交？"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(!ok) return;
    model->database().transaction(); //开始事务操作
       if (model->submitAll()) {
          model->database().commit(); //提交
       } else {
          model->database().rollback(); //回滚
          QMessageBox::critical(this, tr("错误"),
                                tr("数据库错误: %1")
                                .arg(model->lastError().text()));
       }
}

void MainWindow::onTableClicked(const QModelIndex &index){
    if(index.isValid()&&index.column()==7){
        InfoDialog *dlg=new InfoDialog(this, model->record(index.row()),state,pr,model1);
        dlg->exec();
        delete dlg;
    }
}

void MainWindow::on_cancelButton_clicked()
{
    model->revertAll();
}


void MainWindow::on_deleteButton_clicked()
{
    //获取选中的行
    int curRow = ui->tableView_2->currentIndex().row();
    //获取该记录id
    int id = model->record(curRow).value(0).toInt();
    int ok = QMessageBox::warning(this,tr("删除当前行!"),tr("你确定"
                                                     "删除当前行吗？"),
                                      QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
    {
        model->revertAll(); //如果不删除，则撤销
        return;
    }
    //删除该行
    model->removeRow(curRow);
    model->submitAll();
    QString str=model->filter();
    model->setFilter("");
    model->setSort(0,Qt::AscendingOrder); //按id升序排列
    model->select();
    k=model->rowCount();
    for(int i=id; i<k; i++){
        model->setData(model->index(i,0),i);
    }
    model->submitAll();
    model->setFilter(str);
    model->select();
}

void MainWindow::on_discountButton_clicked()
{
    DiscountDialog *mydialog = new DiscountDialog(this);
    Qt::WindowFlags flags=mydialog->windowFlags();
    mydialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); //设置窗口为固定大小
    int ret=mydialog->exec () ;
    if(ret==QDialog::Accepted){
        int type = mydialog->getType();
        int num = mydialog->getNumber();
        QString str=model->filter();
        model->setFilter(QString("(type='%1')AND(owner='%2')").arg(QString::number(type)).arg(pr->account));
        model->select();
        int n = model->rowCount();
        for(int i=0; i<n ; i++){
            model->setData(model->index(i,4), num/10.0*model->record(i).value(4).toDouble());
        }
        model->submitAll();
        model->setFilter(str);
        model->select();
    }
    delete mydialog;
}

void MainWindow::on_deleteButton1_clicked()
{
    //获取选中的行
    int curRow = ui->tableView_3->currentIndex().row();
    //获取该记录id
    int id = model1->record(curRow).value(0).toInt();
    int ok = QMessageBox::warning(this,tr("删除当前行!"),tr("你确定"
                                                     "删除当前行吗？"),
                                      QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
    {
        model1->revertAll(); //如果不删除，则撤销
        return;
    }
    //删除该行
    model1->removeRow(curRow);
    model1->submitAll();
    QString str=model1->filter();
    model1->setFilter("");
    model1->setSort(0,Qt::AscendingOrder); //按id升序排列
    model1->select();
    k1=model1->rowCount();
    for(int i=id; i<k1; i++){
        model1->setData(model1->index(i,0),i);
    }
    model1->submitAll();
    model1->setFilter(str);
    model1->select();
}

void MainWindow::on_cancelButton1_clicked()
{
    model1->revertAll();
}

void MainWindow::on_submitButton1_clicked()
{
    int ok = QMessageBox::warning(this,tr("警告"),tr("确定提交？"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(!ok) return;
    model1->database().transaction(); //开始事务操作
       if (model1->submitAll()) {
          model1->database().commit(); //提交
       } else {
          model1->database().rollback(); //回滚
          QMessageBox::critical(this, tr("错误"),
                                tr("数据库错误: %1")
                                .arg(model1->lastError().text()));
       }
}

void MainWindow::on_orderButton_clicked()
{
    if(bill){
        QMessageBox::warning(this, "警告","您还有未支付的订单");
        return;
    }
    QString s = model->filter();
    for(int i=0; i<model1->rowCount(); i++){
        QSqlRecord record = model1->record(i);
        model->setFilter(QString("id='%1'").arg(record.value(5).toInt())); //在商品表中寻找对应的商品
        model->select();
        QSqlRecord record1 = model->record(0);
        int number = record.value(3).toInt(), remain = record1.value(5).toInt();
        if(number>remain){ //订单生成失败
            sum=0;
            QMessageBox::critical(this, "错误","订单生成失败");
            return;
        }
    }
    for(int i=0; i<model1->rowCount(); i++){
        QSqlRecord record = model1->record(i);
        model->setFilter(QString("id='%1'").arg(record.value(5).toInt())); //在商品表中寻找对应的商品
        model->select();
        QSqlRecord record1 = model->record(0);
        int number = record.value(3).toInt(), remain = record1.value(5).toInt();
        sum+=number * record.value(2).toDouble();
        model->setData(model->index(0,5),remain-number);
        model->submitAll();
    }
    model->setFilter(s);
    model->select();
    bill=true;
    QMessageBox::information(this, "提示", "订单生成成功");
    ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置订单信息不可编辑
}

void MainWindow::on_pushButton_clicked() //查看订单
{
    if(!bill){
        QMessageBox::warning(this, "警告", "您还没有订单");
        return;
    }
    ui->stackedWidget->setCurrentIndex(5);
    ui->textBrowser->setText(QString("%1").arg(sum));
    ui->tableView_4->setModel(model1);
    ui->tableView_4->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置订单信息不可编辑
    ui->tableView_4->setColumnHidden(0,1);
    ui->tableView_4->setColumnHidden(4,1);
    ui->tableView_4->setColumnHidden(5,1);
    ui->tableView_4->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::on_pushButton_2_clicked() //支付
{
    if(!isconnect){
        QMessageBox::warning(this,"警告","未与服务器连接");
        return;
    }
    if(pr->balance<sum){
        QMessageBox::warning(this, "警告", "余额不足");
        return;
    }
    pr->balance-=sum;
    changeFile(0,2,this,pr->account,QString("%1").arg(sum),0); //改变用户余额
    //将钱转入商家余额中
    for(int i=0; i<model1->rowCount(); i++){
        model->setFilter(QString("id='%1'").arg(model1->record(i).value(5).toInt())); //在商品表中寻找对应的商品
        model->select();
        QSqlRecord record1 = model->record(0);
        QString name = record1.value(6).toString(); //找到对应的商家
        double n = model1->record(i).value(2).toDouble();
        int p = model1->record(i).value(3).toInt();
        n*=p;
        changeFile(1,2,this,name,QString("%1").arg(n),1);
    }
    QMessageBox::information(this,"提示","支付成功");
    ui->tableView_3->setEditTriggers(QAbstractItemView::DoubleClicked);
    bill=false;
    sum=0;
}

void MainWindow::on_pushButton_3_clicked() //取消订单
{
    if(!bill) return;
    bill=false;
    sum=0;
    QString s = model->filter();
    for(int i=0; i<model1->rowCount(); i++){
        QSqlRecord record = model1->record(i);
        model->setFilter(QString("id='%1'").arg(record.value(5).toInt())); //在商品表中寻找对应的商品
        qDebug()<<record.value(5).toInt();
        model->select();
        QSqlRecord record1 = model->record(0);
        int number = record.value(3).toInt(), remain = record1.value(5).toInt();
        qDebug()<<remain<<" "<<number;
        model->setData(model->index(0,5),remain+number);
        model->submitAll();
    }
    model->setFilter(s);
    model->select();
    ui->tableView_3->setEditTriggers(QAbstractItemView::DoubleClicked);
}

void MainWindow::on_pushButton_4_clicked() //返回
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if( QMessageBox::question(this,tr("退出"),tr("你确定要退出吗？"),QMessageBox::Yes, QMessageBox::No )== QMessageBox::Yes){
        if(!isconnect){
            delete model;
            delete model1;
            QSqlDatabase db;
            db.setDatabaseName("database.db");
            db.close();
            QSqlDatabase::removeDatabase("qt_sql_default_connection");
            QFile::setPermissions("database.db",QFile::ReadOther | QFile::WriteOther);
            QFile::remove("database.db");
            event->accept();
            return;
        }
        if(bill){ //如果程序退出时还有未支付的订单，则自动取消订单，并解冻商品。
            for(int i=0; i<model1->rowCount(); i++){
                QSqlRecord record = model1->record(i);
                model->setFilter(QString("id='%1'").arg(record.value(5).toInt())); //在商品表中寻找对应的商品
                model->select();
                QSqlRecord record1 = model->record(0);
                int number = record.value(3).toInt(), remain = record1.value(5).toInt();
                model->setData(model->index(0,5),remain+number);
                model->submitAll();
            }
        }
        delete model;
        delete model1;
        QSqlDatabase db;
        db.setDatabaseName("database.db");
        db.close();
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
        //向服务器发数据库文件
        QFile file;
        file.setFileName("database.db");
        //打开文件
        bool ok = file.open(QIODevice::ReadOnly);
        if(!ok){
            QMessageBox::critical(this,"错误","文件读取出错");
            return;
        }
        //开始发文件
        qint64 len=0;
        do{
            char buf[4*1024] = {0};
            len = file.read(buf, sizeof(buf));
            len = conn->write(buf, len);
        }while(len>0);
        loop.exec(QEventLoop::ExcludeUserInputEvents);
        file.close();
        QFile::setPermissions("database.db",QFile::ReadOther | QFile::WriteOther);
        QFile::remove("database.db");
        event->accept();
    }else{
        event->ignore();
    }
}
