#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "changefile.h"
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

int k; //记录模型中的商品数量

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //设置初始状态为未登录
    state=0;
    ui->setupUi(this);
    //设置背景图片
    QPixmap pixmap = QPixmap(":/Images/background.jpg").scaled(this->size());
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette(palette);
    //开始时进入登录页面并加载数据
    ui->stackedWidget->setCurrentIndex(0);
    model = new QSqlTableModel(this);
    model->setTable("goods");
    model->select();
    k=model->rowCount();
    model->setHeaderData(1,Qt::Horizontal,"种类");
    model->setHeaderData(2,Qt::Horizontal,"名称");
    model->setHeaderData(3,Qt::Horizontal,"描述");
    model->setHeaderData(4,Qt::Horizontal,"价格");
    model->setHeaderData(5,Qt::Horizontal,"剩余量");
    model->setHeaderData(6,Qt::Horizontal,"商家");
    //设置下拉框
    QStringList strlist;
    strlist<<"消费者"<<"商家";
    ui->comboBox->addItems(strlist);
    //设置菜单栏槽函数
    connect(ui->actionlogin,&QAction::triggered,[=](){ //登录
        if(state==0){
            ui->stackedWidget->setCurrentIndex(0);
        }else{
            QMessageBox::warning(this,"警告","您已登录");
        }
    });

    connect(ui->actionmanagement,&QAction::triggered,[=](){ //商品管理
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
        //ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置商品信息不可编辑
        ui->tableView->verticalHeader()->setVisible(false);
        ui->tableView->horizontalHeader()->setVisible(false);
    });

    connect(ui->actionlogout,&QAction::triggered,[=](){ //注销
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
        if(state){
            bool ok;
            QString text = QInputDialog::getText(this, "修改密码",
                                                       "新密码:", QLineEdit::Normal,
                                                       QDir::home().dirName(), &ok);
            if (ok){
                if(text!=""){
                    if(text.indexOf(" ")!=-1){
                        QMessageBox::warning(this,"警告","密码不能包含空格");
                    }else{
                        int index=pr->getUserType();
                        pr->password=text;
                        changeFile(index,1,this,text);
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
                    changeFile(index,2,this,QString("%1").arg(pr->balance));
                    QMessageBox::information(this,"提示","充值成功");
                }
            }
        }else{
            QMessageBox::warning(this,"警告","您还未登录");
        }
    });

    connect(ui->actionsearch,&QAction::triggered,[=](){ //搜索
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
            //ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置商品信息不可编辑
            ui->tableView->verticalHeader()->setVisible(false);
            ui->tableView->horizontalHeader()->setVisible(false);
        }
        delete dlg;
    });

    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete pr;
    delete model;
}

void MainWindow::on_LoginButton_clicked()
{
    QString a=ui->Account->text(),p=ui->Password->text();
    if(a!=""&&p!=""){
        QString path;
        int index=ui->comboBox->currentIndex();
        if(index==0){
            path="userdata/consumer.txt";
        }else{
            path="userdata/seller.txt";
        }
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
             QMessageBox::critical(this,"错误","文件读取出错");
             return;
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
            ui->stackedWidget->setCurrentIndex(3);
            ui->label_4->setText(a);
            QPalette pe;
            pe.setColor(QPalette::WindowText,Qt::white);
            ui->label_3->setPalette(pe);
            ui->label_4->setPalette(pe);
            state=1;
            if(index==0){
                pr=new Consumer(a,p,c,0);
            }else{
                pr=new Seller(a,p,c,1);
            }
        }else{
            QMessageBox::warning(this,"警告","账号或密码错误");
        }
        file.close();
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
        QString path;
        if(ui->comboBox->currentIndex()==0){
            path="userdata/consumer.txt";
        }else{
            path="userdata/seller.txt";
        }
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
             QMessageBox::critical(this,"错误","文件读取出错");
             return;
        }
        //检查账号是否被注册过
        QTextStream in(&file);
        QString l="";
        while(!in.atEnd()){
            QString line=in.readLine(0);
            int i=line.indexOf(" ");
            l=line.left(i);
            qDebug()<<l;
            if(l==a){
                break;
            }
        }
        if(l==a){
            QMessageBox::warning(this,"警告","此账号已被注册");
            return;
        }
        file.close();
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)){
             QMessageBox::critical(this,"错误","文件读取出错");
             return;
        }
        //写入文件
        QTextStream out(&file);
        out<<a<<" "<<p<<" "<<"0"<<endl;
        out.flush();
        file.close();
        QMessageBox::information(this,"提示","注册成功");
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
        InfoDialog *dlg=new InfoDialog(this, model->record(index.row()));
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
    k--;
    QString str=model->filter();
    model->setFilter("");
    model->setSort(0,Qt::AscendingOrder); //按id升序排列
    model->select();
    for(int i=id; i<k; i++){
        model->setData(model->index(i,0),i);
    }
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
