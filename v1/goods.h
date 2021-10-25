#ifndef GOODS_H
#define GOODS_H


class Goods
{
public:
    Goods(double p);
    virtual double getPrice();
    double price;
};

#endif // GOODS_H
