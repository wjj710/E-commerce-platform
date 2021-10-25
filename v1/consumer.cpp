#include "consumer.h"

Consumer::Consumer(QString a, QString p, double b, int t):User(a,p,b,t)
{

}

int Consumer::getUserType()const{
    return 0;
}
