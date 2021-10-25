#ifndef FOOD_H
#define FOOD_H
#include "goods.h"

class Food : public Goods
{
public:
    Food(double p);
    double getPrice();
};

#endif // FOOD_H
