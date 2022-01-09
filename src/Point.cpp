#include "Point.h"
#include <math.h>

Point::Point()
{
    this->x = 0.00;
    this->y = 0.00;
    this->z = 0.00;

    this->r = 0.0;
    this->g = 0.0;
    this->b = 0.0;
}

Point::Point(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;

    this->r = 1.0;
    this->g = 1.0;
    this->b = 1.0;
}

Point::~Point()
{

}

double Point::getDistance(Point p)
{
    double x = this->x - p.x;
    double y = this->y - p.y;
    double d = sqrt(x*x + y*y);
    return d;
}
