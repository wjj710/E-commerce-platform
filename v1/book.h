#ifndef BOOK_H
#define BOOK_H
#include "goods.h"


class Book : public Goods
{
public:
    Book(double p);
    double getPrice();
};

#endif // BOOK_H
