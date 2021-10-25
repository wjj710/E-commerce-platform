#include "seller.h"

Seller::Seller(QString a, QString p, double b, int t):User(a,p,b,t)
{

}

int Seller::getUserType(){
    return 1;
}
