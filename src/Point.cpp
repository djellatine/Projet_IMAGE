#include "Point.h"
#include <math.h>

Point::Point()
{
    this->x=0;
    this->y=0;
    this->z=0;
    this->r=0;
    this->g=0;
    this->b=0;
}

Point::Point(double x ,double y, double z)
{
    this->x=x;
    this->y=y;
    this->z=z;
    this->r=1;
    this->g=1;
    this->b=1;
}
Point::~Point()
{
}

double Point::getDistance(Point p)
{
    double x= this->x-p.x;
    double y= this->y-p.y;
    double d=sqrt(x*x+y*y);

    return d;
}
