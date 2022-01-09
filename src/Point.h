#ifndef POINT_H
#define POINT_H

/* classe permettant de représenter de façon concrète un point */
class Point
{
    public :

        Point();
        Point(double x, double y, double z);
        virtual ~Point();
        double getDistance(Point p);

        // coordonnées x, y et z du point
        // (respectivement abscisse, ordonnée, cote)
        double x;
        double y;
        double z;

        // couleur r, v et b du point
        // (respectivement les composantes RVB, soit rouge, vert, bleu)
        float r;
        float g;
        float b;
};

#endif // POINT_H
