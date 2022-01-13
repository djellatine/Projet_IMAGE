/********************************************************/
/*                      sujet_2.cpp                     */
/********************************************************/
/*         Construction en fil de fer de surface        */
/********************************************************/

/* inclusion des fichiers d'en-tête freeglut */

/* Pour les systèmes sous Mac OS X */
#ifdef __APPLE__
#include <GLUT/glut.h>
/* Pour les systèmes sous Windows */
#elif __WIN32__
#pragma comment (lib, "jpeg.lib")
/* Pour les autres systèmes comme Linux */
#else // <=> #elif __GNUC__
#include <GL/glut.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <GL/freeglut.h>
#include <jpeglib.h>
#include <jerror.h>
#include <math.h>
#include "./src/Point.h"

/* définition de la constante pi étant égale à 3.141592... */
#define PI 3.141592

/* declaration des caractéristiques de l'algorithme de De Casteljau */
// degré de la courbe de Bézier sur la longueur
const int m = 2;
// degré de la courbe de Bézier sur la largeur
const int n = 2;
// nombre de récursion à effectuer avant l'arrêt de l'algorithme
const int nbRec = 50;

/* configuration d'une courbe de Bézier */
// initialisation des points de contrôle
Point pc[] =
{
    Point(),
    Point(0.3, 1.0, 0.0),
    Point(2.0, 0.5, 0.0),
    Point(2.5, -1.5, 0.0)
};
// déclaration du tableau de points de la courbe de Bézier
Point tab[nbRec+1];
// déclaration du tableau de points de la courbe fractale
Point fractal[10*nbRec+1];

/* configuration d'une surface de Bézier */
// déclaration du polygone de contrôle
Point polyC[n+1][m+1];
// déclaration du tableau de points constituant la surface de Bézier
Point surface[nbRec+1][nbRec+1];
// déclaration des points de contrôle pour la fractale
Point a, b;

/* fixation de la largeur, de la hauteur, du nombre de bytes par pixel ainsi que de la dimension totale d'une image */
int width = 256, height = 256;
int bpp = 3;
const int dim = width*height*bpp;

/* déclaration d'images de taille 256 par 256 pixels pour pouvoir texturer certains objets de la scène */
unsigned char* img1 = new unsigned char[dim];
unsigned char* img2 = new unsigned char[dim];

/* création d'un tableau d'objets de texture */
GLuint textureObject[2];

/* détermine si le bouton de la souris a été pressé ou non (fait office de booléen) */
char push;

/* angle (en degrés) du champ de vision de l'oeil humain */
double zoom = 70.0;

/* drapeau décisionnaire pour le lancement (dans deux sens différents) ou l'arrêt de l'animation */
int flag = 0;

/* drapeau décisionnaire pour le choix de l'affichage */
int choice = 0;

/* durée (en secondes) du temps qui passe pour les animations de la scène en temps réel */
double theTime = 0.00;

/* durée (en secondes) du temps qui passe pour l'animation du soleil en temps réel */
double theLightTime = (-theTime);

/* ensemble des coordonnées utiles pour fixer la position, la direction ainsi que l'orientation de la caméra */
float posX = -5.0, posY = 10.0, posZ = 15.0, dirX = 5.0, dirY = 2.0, dirZ = 1.0, oriX = 0.0, oriY = 1.0, oriZ = 0.0;

/* angles de rotation de l'objet de la scène ainsi que les valeurs actuelles et anciennes de la position de la souris */
int anglex, angley, x, y, xold, yold;

/* prototype des fonctions */
int mod(int a, int b); // le calcul du modulo
unsigned int facto(int k); // la fonction factorielle
unsigned int miniFacto(int k, int i); // la factorielle version 2
double bernstein(int i, int n, double t); // le polynome de Bernstein
void deCasteljau(); // l'algorithme de De Casteljau
void bezierCurve(); // la courbe de Bézier quadratique
void initTab(); // le tableau de points de contrôle
void bezierSurface(); // la surface de Bézier quadratique
void drawBezierSurface(); // le dessin de la surface de Bézier quadratique
double weierstrass(double x); // la fonction de Weierstrass
void weierstrassFractal(); // la structure fractale de Weierstrass
void drawWeierstrassFractal(); // le dessin de la structure fractale de Weierstrass
void bezierFractalSurface(); // la surface Bézier x Fractale
void drawBezierFractalSurface(); // le dessin de la surface Bézier x Fractale
void draw(); // le dessin des courbes et surfaces désirées
void ground(); // l'affichage du sol (facultatif)
void referenceAxes(); // l'affichage des axes de référence
void initLight(); // la création et l'initialisation des lumières
void idleLight(); // la rotation au ralenti autour de la scène avec une lumière jaune
void display(); // l'affichage de la scène entière
void animate(); // la mise en mouvement de la scène via une animation en fonction du temps
void loadJpegImage(char* imgFile, int* w, int* h, int* b); // le chargement des images .jpeg pour texturer quelques objets
void reshape(int x, int y); // le refenêtrage de la fenêtre principale
void keyboard(unsigned char key, int x, int y); // les fonctions clavier (touches classiques)
void specialKeyboard(int key, int x, int y); // les fonctions clavier (touches spéciales)
void mouse(int button, int state, int x, int y); // la fonction souris (cliques des touches)
void mouseMotion(int x, int y); // la fonction mouvements de souris (déplacements dans l'espace 3D)

/* fonction principale du programme */
int main(int argc, char **argv)
{
    /* permet de lancer le générateur de nombre aléatoire à partir du germe par défaut, ici time(NULL) */
	srand(time(NULL));

    /* chargement de plusieurs textures */
    loadJpegImage((char*) "./img/grass.jpg", &width, &height, &bpp);
    //loadJpegImage((char*) "./img/sand.jpg", &width, &height, &bpp);
    //loadJpegImage((char*) "./img/zebra.jpg", &width, &height, &bpp);
    //loadJpegImage((char*) "./img/checkerboard.jpg", &width, &height, &bpp);

    /* initialisation d'une session GLUT et création de la fenêtre */
    glutInit(&argc, argv); // initialise la bibliothèque glut
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // initialise les différents modes d'affichage
    glutInitWindowPosition(200, 200); // initialise la position de la fenêtre
    glutInitWindowSize(1000, 1000); // initialise la taille de la fenêtre
    glutCreateWindow("Projet MGSI - Sujet 2"); // créé la fenêtre dont le nom est "Projet MGSI - Sujet 2"

    /* initialisation de paramètres tels que l'éclairage */
    initLight();

    /* initialisation d'OpenGL */
    glClearColor(0.0, 0.0, 0.0, 0.0); // détermine la couleur de fond en noir (background)
    glColor3f(1.0, 1.0, 1.0); // fixe la couleur courante à blanc
    glPointSize(2.0); // définit la taille des points à 2.0
    glEnable(GL_DEPTH_TEST); // active le test de profondeur

    /* enregistrement des fonctions de rappel */
    /* fonctions définissant la scène 3D, son animation et sa projection */
    glutDisplayFunc(display);
    glutIdleFunc(animate);
    glutReshapeFunc(reshape);

    /* fonctions liées aux intéractions avec l'utilisateur */
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    /* boucle de traitements des évènements */
    /* entrée et lancement dans la boucle principale glut */
    glutMainLoop();

    /* sortie de la fonction main et fin du programme */
    return 0;
}

/* fonction permettant de trouver le modulo entre 2 nombres entiers */
int mod(int a, int b)
{
    int r = (a%b);

    if(r>=0)
        return r;
    else
        return b+r;
}

/* fonction calculant la factorielle d'un nombre entier */
unsigned int facto(int k)
{
    if ( (k==0) || (k==1) )
        return 1;
    else
        return k*facto(k-1);
}

/* fonction calculant k!/(k-i)! (propriété importante de la factorielle) */
unsigned int miniFacto(int k, int i)
{
    if (k==i)
        return 1;
    else
        return k*miniFacto(k-1, i);
}

/* fonction qui calcule le polynome de Bernstein Bi,n(t) */
double bernstein(int i, int n, double t)
{
    // combinaison Cn,i (coefficients binomiaux)
    unsigned int c = miniFacto(n, n-i)/facto(i);
    double b = c*pow(1-t, n-i)*pow(t, i);
    return b;
}

/* fonction utilisant le polynome de Bernstein ainsi que l'algorithme de De Casteljau
    pour construire une courbe de Bézier quadratique (de degré 2) */
void deCasteljau()
{
    for (int u=0; u<=nbRec; u++)
    {
        // si u=0, alors t=0
        // si u=nbRec, alors t=1
        // sachant que t € [0, 1]
        double t = (double) u/(double) nbRec;
        double x = 0, y = 0, z = 0;
        double b = 0;

        for (int i=0; i<=n; i++)
        {
            b = bernstein(i, n, t);
            //printf("%.2f %.2f %.2f \n", pc[i].x, pc[i].y, pc[i].z);
            x += b*pc[i].x;
            y += b*pc[i].y;
            z += b*pc[i].z;
        }

        // on met le point dans la position u du tableau de la courbe
        tab[u] = Point(x, y, z);
    }
}

/* fonction qui affiche la courbe de Bézier quadratique (de degré 2) à l'aide de l'algorithme de De Casteljau */
void bezierCurve()
{
    deCasteljau();

    for (int i=0; i<=n; i++)
    {
        glPointSize(5);
        glBegin(GL_POINTS);
            glColor3f(1.0, 0.0, 0.0);
            glVertex3d(pc[i].x, pc[i].y, pc[i].z);
        glEnd();
    }

    for (int i=0; i<nbRec; i++)
    {
        glBegin(GL_LINES);
            glColor3f(tab[i].r, tab[i].g, tab[i].b);
            glVertex3f(tab[i].x, tab[i].y, tab[i].z);
            glVertex3f(tab[i+1].x, tab[i+1].y, tab[i+1].z);
        glEnd();
    }
}

/* fonction initialisant le tableau de tous les points de contrôle */
void initTab()
{
    // points de contrôle pour la structure fractale de Weierstrass
    a = Point(-10.0, 0.0, 0.0);
    b = Point(-1.0, 0.0, 0.0);

    polyC[0][0] = Point(0.0, 0.0, 3.0);
    polyC[0][1] = Point(0.0, 1.0, 5.0);
    polyC[0][2] = Point(0.0, 2.0, 2.5);
    //polyC[0][3] = Point(0.0, 3.0, 3.0);

    polyC[1][0] = Point(1.0, 0.0, 2.0);
    polyC[1][1] = Point(1.0, 1.0, 1.5);
    polyC[1][2] = Point(1.0, 2.0, 2.5);
    //polyC[1][3] = Point(1.0, 3.0, 1.0);

    polyC[2][0] = Point(2.0, 0.0, 2.0);
    polyC[2][1] = Point(2.0, 1.0, 3.5);
    polyC[2][2] = Point(2.0, 2.0, 2.5);
    //polyC[2][3] = Point(2.0, 3.0, 1.5);

    /*
    polyC[3][0] = Point(3.0, 0.0, 3.5);
    polyC[3][1] = Point(3.0, 1.0, 1.5);
    polyC[3][2] = Point(3.0, 2.0, 2.0);
    polyC[3][3] = Point(3.0, 3.0, 4.0);
    */
}

/* fonction faisant le produit tensoriel entre 2 courbes de Bézier quadratique (de degré 2) */
void bezierSurface()
{
    initTab();

    // on fixe d'abord j et on fait varier i
    for (int l=0; l<=nbRec; l++)
    {
        double v = (double) l/(double) nbRec;

        for (int k=0; k<=nbRec; k++)
        {
            double u = (double) k/(double) nbRec;
            double xij = 0, yij = 0, zij = 0;
            double bj;

            for (int j=0; j<=n; j++)
            {
                bj = bernstein(j, n, v);

                double xi = 0, yi = 0, zi = 0;
                double bi;

                for (int i=0; i<=m; i++)
                {
                    bi = bernstein(i, m, u);

                    xi += bi*polyC[i][j].x;
                    yi += bi*polyC[i][j].y;
                    zi += bi*polyC[i][j].z;
                }

                xij += bj*xi;
                yij += bj*yi;
                zij += bj*zi;
            }

            surface[k][l] = Point(xij, yij, zij);
        }
    }
}

/* fonction qui affiche la surface de Bézier quadratique (de degré 2) */
void drawBezierSurface()
{
    bezierSurface();

    // affichage du polygone de contrôle
    for (int i=0; i<=n; i++)
    {
        for (int j=0; j<=m; j++)
        {
            glPointSize(8);
            glBegin(GL_POINTS);
                glColor3f(1.0, 0.0, 0.0);
                glVertex3f(polyC[i][j].x, polyC[i][j].y, polyC[i][j].z);
            glEnd();
            glLineWidth(3);

            if(i!=n)
            {
                glBegin(GL_LINES);
                    glColor3f(0.0, 1.0, 0.0);
                    glVertex3f(polyC[i][j].x, polyC[i][j].y, polyC[i][j].z);
                    glVertex3f(polyC[i+1][j].x, polyC[i+1][j].y, polyC[i+1][j].z);
                glEnd();
            }

            if(j!=m)
            {
                glBegin(GL_LINES);
                    glColor3f(0.0, 1.0, 0.0);
                    glVertex3f(polyC[i][j].x, polyC[i][j].y, polyC[i][j].z);
                    glVertex3f(polyC[i][j+1].x, polyC[i][j+1].y, polyC[i][j+1].z);
                glEnd();
            }
        }
    }

    // affichage de la surface de Bézier
    for (int i=0; i<=nbRec; i++)
    {
        for (int j=0; j<=nbRec; j++)
        {
            glLineWidth(1);

            if(j!=nbRec)
            {
                glBegin(GL_LINES);
                    glColor3f(surface[i][j].r, surface[i][j].g, surface[i][j].b);
                    glVertex3d(surface[i][j].x, surface[i][j].y, surface[i][j].z);
                    glVertex3d(surface[i][j+1].x, surface[i][j+1].y, surface[i][j+1].z);
                glEnd();
            }

            if(i!=nbRec)
            {
                glBegin(GL_LINES);
                    glColor3f(surface[i][j].r, surface[i][j].g, surface[i][j].b);
                    glVertex3d(surface[i][j].x, surface[i][j].y, surface[i][j].z);
                    glVertex3d(surface[i+1][j].x, surface[i+1][j].y, surface[i+1][j].z);
                glEnd();
            }
        }
    }
}

/* fonction effectuant le calcul de Weierstrass */
double weierstrass(double x)
{
    double y = 0;
    double a, b;

    for (int k=0; k<=nbRec; k++)
    {
        a = pow(0.75, k);
        b = pow(1.5, k);

        y += a*sin(PI*b*x);
    }

    return y;
}

/* fonction qui construit la structure fractale de Weierstrass */
void weierstrassFractal()
{
    fractal[0] = a;
    fractal[10*nbRec] = b;

    // distance entre chaque pas
    double x, dt = (b.x-a.x)/(10*nbRec);

    for (int i=1; i<10*nbRec; i++)
    {
        // on ajoute le nombre de pas à l'ancienne valeur de x
        x = fractal[i-1].x+dt;
        fractal[i] = Point(x, weierstrass(x), 0);
    }
}

/* fonction qui affiche la structure fractale de Weierstrass */
void drawWeierstrassFractal()
{
    initTab();
    weierstrassFractal();

    glPointSize(4);
    glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(a.x, a.y, a.z);
        glVertex3d(b.x, b.y, b.z);
    glEnd();

    for (int i=0; i<10*nbRec; i++)
    {
        glBegin(GL_LINES);
            glColor3f(fractal[i].r, fractal[i].g, fractal[i].b);
            glVertex3d(fractal[i].x, fractal[i].y, fractal[i].z);
            glColor3f(fractal[i+1].r, fractal[i+1].g, fractal[i+1].b);
            glVertex3d(fractal[i+1].x, fractal[i+1].y, fractal[i+1].z);
        glEnd();
    }
}

/* fonction faisant le produit tensoriel entre une courbe de Bézier et la fractale de Weierstrass */
void bezierFractalSurface()
{
    initTab();

    // on fixe d'abord j et on fait varier i
    for (int l=0; l<=nbRec; l++)
    {
        // pour la colonne...
        double v = (double) l/ (double) nbRec;

        for (int k=0; k<=nbRec; k++)
        {
            // pour la ligne...
            double u = (double) k/(double) nbRec;
            double xij = 0, yij = 0, zij = 0;
            double bj;

            for (int j=0; j<=n; j++)
            {
                bj = bernstein(j, n, v);

                double xi = 0, yi = 0, zi = 0;
                double bi;

                for (int i=0; i<=m; i++)
                {
                    bi = bernstein(i, m, u);

                    xi += bi*polyC[i][j].x;
                    yi += bi*polyC[i][j].y;
                    zi += bi*polyC[i][j].z;
                }

                if((j%2)==1)
                {
                    xij += bj*xi;
                    yij += bj*weierstrass(xi);
                    zij += bj*weierstrass(xi);
                }

                else
                {
                    xij += bj*xi;
                    yij += bj*yi;
                    zij += bj*zi;
                }
            }

            surface[k][l] = Point(xij, yij, zij);
        }
    }
}

/* fonction affichant la surface de Bézier x Fractale */
void drawBezierFractalSurface()
{
    bezierFractalSurface();

    // affichage du polygone de contrôle
    for (int i=0; i<=n; i++)
    {
        for (int j=0; j<=m; j++)
        {
            glPointSize(8);
            glBegin(GL_POINTS);
                glColor3f(1.0, 0.0, 0.0);
                glVertex3f(polyC[i][j].x, polyC[i][j].y, polyC[i][j].z);
            glEnd();
            glLineWidth(3);

            if(i!=n)
            {
                glBegin(GL_LINES);
                    glColor3f(0.0, 1.0, 0.0);
                    glVertex3f(polyC[i][j].x, polyC[i][j].y, polyC[i][j].z);
                    glVertex3f(polyC[i+1][j].x, polyC[i+1][j].y, polyC[i+1][j].z);
                glEnd();
            }

            if(j!=m)
            {
                glBegin(GL_LINES);
                    glColor3f(0.0, 1.0, 0.0);
                    glVertex3f(polyC[i][j].x, polyC[i][j].y, polyC[i][j].z);
                    glVertex3f(polyC[i][j+1].x, polyC[i][j+1].y, polyC[i][j+1].z);
                glEnd();
            }
        }
    }

    // affichage de la surface de Bézier
    for (int i=0; i<=nbRec; i++)
    {
        for (int j=0; j<=nbRec; j++)
        {
            glLineWidth(1);

            if(j!=nbRec)
            {
                glBegin(GL_LINES);
                    glColor3f(surface[i][j].r, surface[i][j].g, surface[i][j].b);
                    glVertex3d(surface[i][j].x, surface[i][j].y, surface[i][j].z);
                    glVertex3d(surface[i][j+1].x, surface[i][j+1].y, surface[i][j+1].z);
                glEnd();
            }

            if(i!=nbRec)
            {
                glBegin(GL_LINES);
                    glColor3f(surface[i][j].r, surface[i][j].g, surface[i][j].b);
                    glVertex3d(surface[i][j].x, surface[i][j].y, surface[i][j].z);
                    glVertex3d(surface[i+1][j].x, surface[i+1][j].y, surface[i+1][j].z);
                glEnd();
            }
        }
    }
}

/* fonction permettant de tracer en mode fil de fer le dessin de :
    - la courbe de Bézier quadratique (de degré 2)
    - la structure fractale de Weierstrass
    - la surface de Bézier quadratique (de degré 2)
    - la surface Bézier x Fractale
*/
void draw()
{
    /* choix de la construction que l'on souhaite afficher (par défaut, la courbe de Bézier) */
    switch (choice)
    {
        /* affichage de la courbe de Bézier quadratique (de degré 2) */
        case 0 :
            bezierCurve();
            glutPostRedisplay();
            break;

        /* affichage de la courbe fractale de Weierstrass */
        case 1 :
            drawWeierstrassFractal();
            glutPostRedisplay();
            break;

        /* affichage de la surface de Bézier quadratique (de degré 2) */
        case 2 :
            drawBezierSurface();
            glutPostRedisplay();
            break;

        /* affichage de la surface de Bézier x Fractale de Weierstrass */
        case 3 :
            drawBezierFractalSurface();
            glutPostRedisplay();
            break;
    }
}

/* fonction permettant d'afficher le sol de la scène avec une texture sable */
void ground()
{
    glPushMatrix();
        /* génération d'un nouvel objet de texture */
        //glGenTextures(1, &textureObject[0]);

        /* paramétrage du placage de textures (filtrage) */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        /* liaison de la texture avec le fragment */
        //glBindTexture(GL_TEXTURE_2D, textureObject[0]);

        /* définition d'une image 2D comme texture */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img1);

        /* paramétrage de la combinaison de la texture avec les pixels du fragment */
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        /* activation de l'application de la texture 2D par placage */
        glEnable(GL_TEXTURE_2D);

        /* transformations appliquées avant l'application du dessin */
        glColor3f(0.2, 0.5, 0.0);
        glTranslatef(0.0, -0.5, 0.0);
        glScalef(25.0, 0.2, 25.0);

        /* dessin du sol en forme de carré 2D */
        glBegin(GL_POLYGON);
            glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
            glTexCoord2f(0.0, 1.0); glVertex3f(0.5, 0.0, 0.0);
            glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 0.0, 0.5);
            glTexCoord2f(1.0, 0.0); glVertex3f(0.0, 0.0, 0.5);
        glEnd();

        /* désactivation de l'application de la texture 2D par placage */
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/* fonction servant à afficher les 3 axes de références du repère */
void referenceAxes()
{
    /* repère du plan Oxyz */
    // axe x en rouge
    glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
    	glVertex3f(0.0, 0.0, 0.0);
    	glVertex3f(10.0, 0.0, 0.0);
    glEnd();

    // axe des y en vert
    glBegin(GL_LINES);
    	glColor3f(0.0, 1.0, 0.0);
    	glVertex3f(0.0, 0.0, 0.0);
    	glVertex3f(0.0, 10.0, 0.0);
    glEnd();

    // axe des z en bleu
    glBegin(GL_LINES);
    	glColor3f(0.0, 0.0, 1.0);
    	glVertex3f(0.0, 0.0, 0.0);
    	glVertex3f(0.0, 0.0, 10.0);
    glEnd();
}

/* fonction créant et initialisant les lumières (éclairages) */
void initLight()
{
    /* création et initialisation de différents types de lumières (ambiante, diffuse, spéculaire) */
    /* création également de plusieurs faisceaux lumineux de couleurs diverses (rouge, bleu, blanc) */
    const GLfloat ambientLight[4] = { 0.0, 4.0, 0.0, 1.0 };

    const GLfloat diffuseLight[4] = { -5.0, 5.0, 5.0, 0.0 };
    const GLfloat diffLightDir[4] = { 0.0, 2.0, -1.0, 0.0 };

    const GLfloat specularLight[4] = { 6.2, 4.0, 4.0, 0.0 };
    const GLfloat specuLightDir[4] = { 6.2, 1.6, 1.0, 0.0 };

    const GLfloat redLight[4] = { 1.0, 0.0, 0.0, 1.0 };
    const GLfloat blueLight[4] = { 0.0, 0.0, 1.0, 1.0 };
    const GLfloat whiteLight[4] = { 1.0, 1.0, 1.0, 1.0 };

    /* désigniation de la source lumineuse */
    glLightfv(GL_LIGHT0, GL_POSITION, ambientLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, redLight);

    glLightfv(GL_LIGHT1, GL_POSITION, diffuseLight);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, diffLightDir);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, blueLight);

    glLightfv(GL_LIGHT2, GL_POSITION, specularLight);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, specuLightDir);
    glLightfv(GL_LIGHT2, GL_SPECULAR, whiteLight);
}

/* fonction permettant de faire tourner le soleil autour de la scène (lumière jaune) */
void idleLight()
{
    /* création et initialisation d'une lumière de type (ambiante, diffuse, spéculaire) */
    /* création également d'un faisceau lumineux de couleur jaune */
    const GLfloat diffuseLight[4] = { 0.0, 0.0, 3.0, 1.0 };
    const GLfloat diffLightDir[4] = { 0.0, 0.0, -1.0, 0.0 };
    const GLfloat sunLight[4] = { 1.0, 1.0, 0.0, 1.0 };

    /* désigniation de la source lumineuse */
    glLightfv(GL_LIGHT3, GL_POSITION, diffuseLight);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, diffLightDir);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, sunLight);

    /* effet sur les couleurs */
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 100);

    /* activation ou réactivation de l'éclairage */
    glEnable(GL_LIGHTING);

    /* activation de l'application de la lumière jaune */
    glEnable(GL_LIGHT3);

    /* appel de la fonction qui permet le rafraîchissement de l'affichage */
    glutPostRedisplay();
}

/* fonction d'affichage permettant :
    - "d'effacer l'écran" pour l'actualiser
    - de positionner la caméra
    - d'afficher les axes à partir desquels se font la construction
    - de construire/tracer la scène
    - de mettre à jour les buffers
*/
void display()
{
    int w = 1000, h = 1000;

    /* effacement de l'image avec la couleur de fond */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* lissage de surface avec un dégradé de couleur (modèle de Gouraud) */
    glShadeModel(GL_SMOOTH);
    //glShadeModel(GL_FLAT);

    /* sélection de la matrice active avec le passage en mode projection */
    glMatrixMode(GL_PROJECTION);

    /* initialisation de la matrice active */
    glLoadIdentity();

    /* définition d'un volume de vision avec la projection perspective */
    gluPerspective(zoom, (float) w / (float) h, 0.1, 40.0);
    //glOrtho(-20, 20, -20, 20, -30, 30); transformation pour la projection parallèle (mode orthogonal)

    /* sélection de la matrice active avec le passage en mode modèle de vue */
    glMatrixMode(GL_MODELVIEW);

    /* initialisation de la matrice active */
    glLoadIdentity();

    /* fixation de la position de la caméra, repère de l'oeil pour l'utilisateur */
    gluLookAt(posX, posY, posZ, dirX, dirY, dirZ, oriX, oriY, oriZ);
    //gluLookAt(1.5, 3.0, 3.0, 2.0, 2.0, 0.0, 0.0, 1.0, 0.0); placement de la caméra en projection parallèle

    /* changement d'échelle en 3D */
    glScalef(2.0, 2.0, 1.0);

    /* rotation en 3D selon un axe et un angle */
    glRotatef(angley, 1.0, 0.0, 0.0);
    glRotatef(anglex, 0.0, 1.0, 0.0);

    /** dessin de la scène **/
    /* affichage des 3 axes du plan 3D */
    referenceAxes();

    /* rotation de la scène inverse au sens de rotation du soleil */
    glRotatef(theTime, 0.0, 1.0, 0.0);

    /* affichage du sol texturé */
    //ground();

    /* affichage des courbes et surfaces en fil de fer */
    draw();

    /* transformations appliquées avant l'application du dessin */
    glColor3f(0.5, 0.5, 0.5);
    glTranslatef(2.0, 2.0, 2.0);
    //glScalef(25.0, 0.2, 25.0);

    /* appel de la fonction qui permet de gérer le mode double tampons (buffers) */
    glFlush();

    /* appel de la fonction qui permet le rafraîchissement de l'affichage */
    glutPostRedisplay();

    /* appel de la fonction qui échange les tampons (buffers) et appel la fonction glFlush() */
    glutSwapBuffers();
}

/* fonction manipulant le temps en fonction d'un drapeau, pour produire une animation de la scène */
void animate()
{
    /* arrêt du timing servant à animer la scène en exécutant des mouvements en continue (fonction de stop puis de remise en position initiale de la scène) */
    if (flag == 0)
    {
        theTime = 0.00;
    }

    /* mise ou remise en marche du timing servant à animer la scène en continue dans le sens horaire */
    else if (flag == 1)
    {
        theTime -= 1.00;
    }

    /* mise ou remise en marche du timing servant à animer la scène en continue dans le sens anti-horaire (ou trigonométrique) */
    else if (flag == 2)
    {
        theTime += 1.00;
    }

    /* mise ou remise en marche du timing servant à animer le soleil dans le sens opposé à la scène */
    /* pour cela, il suffit d'appuyer plusieurs fois sur la touche "l" du clavier pour que la lumière jaune tourne */
    else
    {
        /* rotation du soleil inverse au sens de rotation de la scène */
        glRotatef(theLightTime, 0.0, 1.0, 0.0);
        theLightTime += 1.00;
    }
}

/* fonction permettant de charger les 2 images .jpeg pour pouvoir ensuite les associer chacune comme texture à un objet */
void loadJpegImage(char* imgFile, int* w, int* h, int* b)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE* file;
    unsigned char* line;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    #ifdef __WIN32
    if (fopen_s(&file, imgFile, "rb") != 0)
    {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier .jpeg !\n");
        exit(1);
    }
    #elif __GNUC__
    if ((file = fopen(imgFile, "rb")) == 0)
    {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier .jpeg !\n");
        exit(1);
    }
    #endif

    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, TRUE);

    std::cout << "Vérification des composantes d'une texture avant son placage sur une surface..." << std::endl;
    std::cout << "Largeur : " << cinfo.image_width << std::endl;
    std::cout << "Hauteur : " << cinfo.image_height << std::endl;
    std::cout << "Numéro de composants : " << cinfo.num_components << std::endl;


    if ( (cinfo.image_width != (unsigned int) *w) || (cinfo.image_height != (unsigned int) *h) )
    {
        fprintf(stdout, "Erreur : l'image doit-être de taille 256x256 !\n");
        exit(1);
    }

    if (cinfo.jpeg_color_space == JCS_GRAYSCALE)
    {
        fprintf(stdout, "Erreur : l'image doit-être de type RGB !\n");
        exit(1);
    }

    jpeg_start_decompress(&cinfo);

    if (imgFile == (char*) "./img/grass.jpg") // OU... "./img/sand.jpg"
    {
        line = img1;

        while (cinfo.output_scanline < cinfo.output_height)
        {
            line = img1 + (*b) * (*w) * cinfo.output_scanline;
            jpeg_read_scanlines(&cinfo, &line, 1);
        }
    }

    else if (imgFile == (char*) "./img/zebra.jpg") // OU... "./img/checkerboard.jpg"
    {
        line = img2;
        while (cinfo.output_scanline < cinfo.output_height)
        {
            line = img2 + (*b) * (*w) * cinfo.output_scanline;
            jpeg_read_scanlines(&cinfo, &line, 1);
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
}

/* fonction servant à redéfinir le cadre d'affichage après le redimensionnement de la fenêtre */
/* définition de la projection 3D vers 2D */
void reshape(int x, int y)
{
    if (x < y)
        glViewport(0, (y-x)/2, x, x);

    else
        glViewport((x-y)/2, 0, y, y);
}

/* fonction permettant la modification du contexte d'affichage sur un évènement clavier (appuie sur une touche "classique") */
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        /* la touche 'r' permet l'affichage en mode rempli/plein */
        case 'r':
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glutPostRedisplay();
            break;

        /* la touche 'f' permet l'affichage en mode fil de fer */
        case 'f':
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glutPostRedisplay();
            break;

        /* la touche 's' permet l'affichage en mode sommets seuls */
        case 's' :
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glutPostRedisplay();
            break;

        /* la touche 'm' permet de paramétrer le placage de textures */
        /* (filtrage "moyenneur" qui fait la moyenne des 4 texels les plus proches) */
        case 'm' :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glutPostRedisplay();
            break;

        /* la touche 'v' permet de paramétrer le placage de textures */
        /* (filtrage "voisinage" qui choisit le texel voisin le plus proche du centre du pixel) */
        case 'v' :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glutPostRedisplay();
            break;

        /* la touche 'j' permet d'éteindre toutes les lumières de la scène (mode jour) */
        case 'j' :
            /* désactivation de l'application des lumières */
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHT1);
            glDisable(GL_LIGHT2);
            glDisable(GL_LIGHT3);
            /* désactivation de l'éclairage */
            glDisable(GL_LIGHTING);
            glutPostRedisplay();
            break;

        /* la touche 'n' permet d'allumer toutes les lumières de la scène (mode nuit) */
        case 'n' :
            /* activation ou réactivation de l'éclairage */
            glEnable(GL_LIGHTING);
            /* activation de l'application des lumières */
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHT1);
            glEnable(GL_LIGHT2);
            glutPostRedisplay();
            break;

        /* la touche 'l' permet de gèrer manuellement la rotation du soleil autour de la scène */
        case 'l' :
            flag = 3;
            /* lancement de l'animation du soleil */
            idleLight();
            glutPostRedisplay();
            break;

        /* la touche '0' met fin à toutes les animations continues en temps réel (touche arrêt animations) */
        case '0' :
            flag = 0;
            glutPostRedisplay();
            break;

        /* la touche '1' redémarre toutes les animations continues en temps réel dans le sens horaire (touche redémarrer animations 1) */
        case '1' :
            flag = 1;
            glutPostRedisplay();
            break;

        /* la touche '2' redémarre toutes les animations continues en temps réel dans le sens anti-horaire ou trigonométrique (touche redémarrer animations 2) */
        case '2' :
            flag = 2;
            glutPostRedisplay();
            break;

        /* la touche '3' désigne le choix d'affichage à zéro, c'est-à-dire que la courbe de Bézier est dessinée sur la scène */
        case '3' :
            choice = 0;
            glutPostRedisplay();
            break;

        /* la touche '4' désigne le choix d'affichage à un, c'est-à-dire que la courbe fractale de Weierstrass est dessinée sur la scène */
        case '4' :
            choice = 1;
            glutPostRedisplay();
            break;

        /* la touche '5' désigne le choix d'affichage à deux, c'est-à-dire que la surface de Bézier est dessinée sur la scène */
        case '5' :
            choice = 2;
            glutPostRedisplay();
            break;

        /* la touche '6' désigne le choix d'affichage à trois, c'est-à-dire que la surface de Bézier x Fractale de Weierstrass est dessinée sur la scène */
        case '6' :
            choice = 3;
            glutPostRedisplay();
            break;

        /* la touche 'z' permet l'affichage en mode zoomé (la caméra se rapproche de l'objet) */
        case 'z' :
            if (zoom > 20)
                zoom -= 10.0;
            glutPostRedisplay();
            break;

        /* la touche 'Z' permet l'affichage en mode dézoomé (la caméra s'éloigne de l'objet) */
        case 'Z' :
            if (zoom < 130)
                zoom += 10.0;
            glutPostRedisplay();
            break;

        /* la touche 'd' permet l'activation des fragments non visibles sur l'affichage */
        case 'd':
            glEnable(GL_DEPTH_TEST);
            glutPostRedisplay();
            break;

        /* la touche 'D' permet la désactivation des fragments non visibles sur l'affichage */
        case 'D':
            glDisable(GL_DEPTH_TEST);
            glutPostRedisplay();
            break;

        /* la touche 'q' permet de quitter le programme */
        case 'q' :
            exit(0);
    }
}

/* fonction permettant la modification du contexte d'affichage sur un évènement clavier (appuie sur une touche "spéciale") */
void specialKeyboard(int key, int x, int y)
{
    /* les touches "flèches vers le haut/bas/gauche/droite" permettent de modifier la vue de l'objet */
    switch (key)
    {
        /* la touche "flèche vers le haut" permet de faire tourner la caméra autour de la fourmi par le bas */
        case GLUT_KEY_UP :
            angley = (angley+5)%360;
            glutPostRedisplay();
            break;

        /* la touche "flèche vers le bas" permet de faire tourner la caméra autour de la fourmi par le haut */
        case GLUT_KEY_DOWN :
            angley = (angley-5)%360;
            glutPostRedisplay();
            break;

        /* la touche "flèche vers la gauche" permet de faire tourner la caméra autour de la fourmi par la droite */
        case GLUT_KEY_LEFT :
            anglex = (anglex+5)%360;
            glutPostRedisplay();
            break;

        /* la touche "flèche vers la droite" permet de faire tourner la caméra autour de la fourmi par la gauche */
        case GLUT_KEY_RIGHT :
            anglex = (anglex-5)%360;
            glutPostRedisplay();
            break;
    }
}

/* fonction permettant la modification du contexte d'affichage sur un évènement souris (avec le bouton pressé ou relâché) */
void mouse(int button, int state, int x, int y)
{
    /* si on appuie sur le bouton gauche */
    if ( (button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN) )
    {
        /* le booléen presse passe a 1 (vrai) */
        push = 1;

        /* on sauvegarde la position de la souris */
        xold = x;
        yold = y;
    }

    /* si on relache le bouton gauche */
    if ( (button == GLUT_LEFT_BUTTON) && (state == GLUT_UP) )
        /* le booléen presse passe a 0 (faux) */
        push = 0;
}

/* fonction permettant la modification du contexte d'affichage lors du déplacement de la souris (avec le bouton pressé) */
void mouseMotion(int x, int y)
{
    /* si le bouton gauche est pressé */
    if (push)
    {
        /* on modifie les angles de rotation de l'objet en fonction de la position actuelle de la souris
        et de la dernière position sauvegardée */
        anglex = anglex+(x-xold);
        angley = angley+(y-yold);

        /* on demande un rafraîchissement de l'affichage */
        glutPostRedisplay();
    }

    /* sauvegarde des valeurs courante de le position de la souris */
    xold = x;
    yold = y;
}
