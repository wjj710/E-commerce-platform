#ifndef SELLER_H
#define SELLER_H
#include "user.h"

class Seller : public User
{
public:
    Seller(QString a, QString p, double b, int t);
    virtual int getUserType();
};

#endif // SELLER_H
