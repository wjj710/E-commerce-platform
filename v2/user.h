#ifndef USER_H
#define USER_H
#include <QString>
#include <QObject>

class User : public QObject
{
    Q_OBJECT

public:
    User(QString a, QString p, double b, int t);
    virtual int getUserType()=0;
    QString account; //账户
    double balance; //余额
    QString password; //密码
private:
    int type; //用户类型
};

#endif // USER_H
