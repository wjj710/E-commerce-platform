#ifndef CLOTH_H
#define CLOTH_H
#include "goods.h"

class Cloth : public Goods
{
public:
    Cloth(double p);
    double getPrice();
};

#endif // CLOTH_H
