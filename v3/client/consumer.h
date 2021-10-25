#ifndef CONSUMER_H
#define CONSUMER_H
#include "user.h"

class Consumer : public User
{
public:
    Consumer(QString a, QString p, double b, int t);
    virtual int getUserType();
};

#endif // CONSUMER_H
